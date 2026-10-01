"""Monte Carlo comparison of card-archetype decks against fixed enemy setups.

Run from the repo root:  python tools/simulate_decks.py

Models the battle rules that matter for damage (checked against IBattleScene.cpp):
  * 3 energy per turn, 5 cards drawn per turn. Unplayed cards are discarded at the end of the turn
    (IBattleScene::MoveHandCardsToDiscardPile), so every turn starts from a fresh 5-card hand.
  * A persistent card is paid for once, then re-executes every turn until the played pile is
    cleared. The pile clears at the start of turns 4, 7, 10... (currentTurn % 3 == 1), so a
    persistent card installed on turn N of a 3-turn cycle runs (4 - N) times.
  * A non-persistent card executes once and goes to the discard pile; it only comes back when the
    draw pile runs out and the discard pile is reshuffled into it.
  * Damage over time ticks at the end of the turn, then durations count down.
  * The player always orders the program sensibly (status setup, then hits, then payoffs), and
    always focus-fires the first living enemy. Enemies neither heal nor defend, and the player
    uses no block or Weakness.

The decks play from simple priority lists - good, not optimal - so read the output as a comparison
between archetypes, not as a prediction of any one run.
"""
import copy
import random
import statistics
import sys

MAX_TURNS = 15
ENERGY = 3
DRAW = 5
CYCLE = 3


# ---- state -------------------------------------------------------------------------------

class Enemy:
    def __init__(self, hp):
        self.hp = float(hp)
        self.max = float(hp)
        self.pdur = 0
        self.burn = 0.0
        self.bdur = 0
        self.mark = 0.0
        self.mdur = 0
        self.spark = 0.0
        self.sdur = 0
        self.vdur = 0

    @property
    def alive(self):
        return self.hp > 0


class Game:
    def __init__(self, hps):
        self.enemies = [Enemy(h) for h in hps]
        self.dealt = 0.0

    def alive(self):
        return [e for e in self.enemies if e.alive]

    def focus(self):
        a = self.alive()
        return a[0] if a else None

    def hit(self, e, base):
        if e is None or not e.alive:
            return
        dmg = base + (e.mark if e.mdur > 0 else 0.0)
        if e.vdur > 0:
            dmg *= 1.5
        dealt = min(dmg, e.hp)
        e.hp -= dmg
        self.dealt += dealt

    def direct(self, e, dmg):
        if e is None or not e.alive:
            return
        dealt = min(dmg, e.hp)
        e.hp -= dmg
        self.dealt += dealt

    def end_of_turn(self):
        for e in self.alive():
            if e.pdur > 0:
                self.direct(e, e.pdur)
            if e.bdur > 0 and e.burn > 0:
                self.direct(e, e.burn)
        for e in self.enemies:
            e.pdur = max(0, e.pdur - 1)
            if e.bdur > 0:
                e.bdur -= 1
                if e.bdur == 0:
                    e.burn = 0.0
            if e.mdur > 0:
                e.mdur -= 1
                if e.mdur == 0:
                    e.mark = 0.0
            if e.sdur > 0:
                e.sdur -= 1
                if e.sdur == 0:
                    e.spark = 0.0
            e.vdur = max(0, e.vdur - 1)


# ---- cards -------------------------------------------------------------------------------
# name: (cost, persistent, order)   order: 0 setup, 1 hits, 2 payoffs

def _strike(dmg):
    return lambda g, c: g.hit(g.focus(), dmg)


def _twin(g, c):
    g.hit(g.focus(), 3)
    g.hit(g.focus(), 3)


def _terminate(g, c):
    f = g.focus()
    if f:
        g.hit(f, 60 if f.hp / f.max < 0.4 else 30)


def _raging(g, c):
    g.hit(g.focus(), 4 + 3 * c['n'])
    c['n'] += 1


def _cleave(g, c):
    for e in g.alive()[:2]:
        g.hit(e, 7)


def _chain_lightning(g, c):
    alive = g.alive()
    if not alive:
        return
    slots = [alive[i] if i < len(alive) else alive[0] for i in range(3)]
    for i, e in enumerate(slots):
        dmg = 20.0
        for j in range(i):
            if slots[j] is e:
                dmg *= 0.25
        g.hit(e, dmg)


def _vulnerable(g, c):
    f = g.focus()
    if f:
        f.vdur = max(f.vdur, 1)


def _apply_marked(e, value, turns=2):
    """Marked takes the strongest source and extra applications extend it (AddModifier)."""
    e.mark = max(e.mark, value)
    e.mdur += turns


def _mark(g, c):
    f = g.focus()
    if f:
        _apply_marked(f, 4)


def _dragnet(g, c):
    for e in g.alive():
        _apply_marked(e, 8)


def _hunter(g, c):
    f = g.focus()
    if f:
        _apply_marked(f, 6)


def _barrage(g, c):
    for _ in range(4):
        g.hit(g.focus(), 3)


def _poison(g, c):
    f = g.focus()
    if f:
        f.pdur += 3


def _toxic_cloud(g, c):
    for e in g.alive():
        e.pdur += 4


def _festering(g, c):
    for e in g.alive():
        if e.pdur > 0:
            e.pdur += 2


def _contagion(g, c):
    f = g.focus()
    if f and f.pdur > 0:
        for e in g.alive():
            if e is not f:
                e.pdur += f.pdur


def _septic(g, c):
    f = g.focus()
    if f:
        g.hit(f, 3 + min(f.pdur, 8))


def _inferno(g, c):
    for e in g.alive():
        g.hit(e, 8)
        if e.alive:
            e.burn += 5
            e.bdur = max(e.bdur, 3)


def _kindle(g, c):
    f = g.focus()
    if f:
        f.burn += 4
        f.bdur = max(f.bdur, 3)


def _fan(g, c):
    for e in g.alive():
        if e.bdur > 0:
            e.burn += 3


def _meltdown(g, c):
    f = g.focus()
    if f:
        b = f.burn
        g.hit(f, 4 + 3 * min(b, 10) + max(0.0, b - 10))


def _ignite(g, c):
    f = g.focus()
    if f:
        f.spark += 3
        f.sdur = max(f.sdur, 3)


def _fire_det(g, c):
    f = g.focus()
    if f:
        s = f.spark
        f.spark, f.sdur = 0.0, 0
        g.hit(f, 4 + 8 * s)


def _short_circuit(g, c):
    f = g.focus()
    if f:
        g.hit(f, 4)
        if f.alive:
            f.spark += 2
            f.sdur = max(f.sdur, 3)


def _static_charge(g, c):
    for e in g.alive():
        e.spark += 2
        e.sdur = max(e.sdur, 3)


def _chain_det(g, c):
    for e in g.alive():
        s = e.spark
        e.spark, e.sdur = 0.0, 0
        g.hit(e, 6 + 8 * s)


CARDS = {
    # physical
    'Jab': (0, True, 1, _strike(3)),
    'Strike': (1, True, 1, _strike(5)),
    'TwinStrike': (1, True, 1, _twin),
    'RagingStrike': (1, True, 1, _raging),
    'HeavyStrike': (2, True, 1, _strike(16)),
    'Cleave': (2, True, 1, _cleave),
    'ChainLightning': (3, True, 1, _chain_lightning),
    'Terminate': (3, True, 1, _terminate),
    'Vulnerable': (1, False, 0, _vulnerable),
    # marked
    'Mark': (0, False, 0, _mark),
    'Dragnet': (1, False, 0, _dragnet),
    'Hunter': (1, True, 0, _hunter),
    'Barrage': (2, False, 1, _barrage),
    # poison
    'Poison': (1, False, 0, _poison),
    'ToxicCloud': (2, False, 0, _toxic_cloud),
    'Festering': (2, True, 0, _festering),
    'Contagion': (1, False, 0, _contagion),
    'SepticStrike': (1, False, 2, _septic),
    # burn
    'Inferno': (3, True, 1, _inferno),
    'Kindle': (1, False, 0, _kindle),
    'FanTheFlames': (1, False, 0, _fan),
    'Meltdown': (2, False, 2, _meltdown),
    # spark
    'Ignite': (1, True, 0, _ignite),
    'ShortCircuit': (1, False, 1, _short_circuit),
    'StaticCharge': (2, True, 0, _static_charge),
    'FireDetonation': (2, False, 2, _fire_det),
    'ChainDetonation': (3, False, 2, _chain_det),
}


def run_program(g, program):
    for c in sorted(program, key=lambda c: CARDS[c['name']][2]):
        CARDS[c['name']][3](g, c)


# ---- decks and policies ------------------------------------------------------------------

def deck(**counts):
    cards = []
    for name, n in counts.items():
        cards += [name] * n
    return cards


def physical():
    return deck(Terminate=2, HeavyStrike=3, RagingStrike=2, TwinStrike=2, Strike=2, Jab=2, ChainLightning=1, Cleave=1)


def poison():
    return deck(Poison=3, ToxicCloud=2, Festering=2, Contagion=1, SepticStrike=3, Strike=2, HeavyStrike=2)


def burn():
    return deck(Inferno=3, Kindle=3, FanTheFlames=2, Meltdown=2, Strike=2, HeavyStrike=2, Terminate=1)


def spark():
    return deck(Ignite=3, FireDetonation=3, ShortCircuit=2, StaticCharge=2, ChainDetonation=2, Strike=1, HeavyStrike=2)


def mark():
    return deck(Mark=3, Dragnet=2, Hunter=2, Barrage=3, TwinStrike=2, Jab=2, Cleave=1)


def with_vulnerable(cards, replace):
    cards = list(cards)
    for name in replace:
        cards.remove(name)
    return cards + ['Vulnerable'] * len(replace)


def mixed():
    return deck(Terminate=1, HeavyStrike=2, Strike=1, Inferno=1, Meltdown=1, Ignite=2, FireDetonation=2,
                Poison=2, SepticStrike=1, Mark=2, Barrage=1)


# Priority lists: first match in hand that passes its condition is played, repeatedly, until
# nothing affordable is left. A bare name has no condition.
def cond_always(g, ctx):
    return True


PRIORITY = {
    'physical': ['Jab', 'Terminate', 'HeavyStrike', 'RagingStrike', 'TwinStrike', 'ChainLightning', 'Cleave', 'Strike'],
    'poison': ['Poison', 'Festering', 'ToxicCloud', 'Contagion', 'SepticStrike', 'HeavyStrike', 'Strike'],
    'burn': ['Inferno', 'Kindle', 'FanTheFlames', 'Meltdown', 'Terminate', 'HeavyStrike', 'Strike'],
    'spark': ['FireDetonation', 'ChainDetonation', 'Ignite', 'StaticCharge', 'ShortCircuit', 'HeavyStrike', 'Strike'],
    'mark': ['Mark', 'Dragnet', 'Hunter', 'Barrage', 'TwinStrike', 'Jab', 'Cleave'],
    'mixed': ['Mark', 'Ignite', 'Poison', 'Inferno', 'FireDetonation', 'Meltdown', 'SepticStrike', 'Barrage',
              'Terminate', 'HeavyStrike', 'Strike'],
}


FILLERS = {'Strike', 'HeavyStrike', 'TwinStrike', 'RagingStrike', 'Cleave', 'Terminate', 'ChainLightning'}


def should_play(name, g, installed_names, rem, multi):
    f = g.focus()
    if f is None:
        return False
    if name == 'Festering':
        return f.pdur > 0 and rem >= 2
    if name == 'Contagion':
        return multi and f.pdur > 0
    if name == 'ToxicCloud':
        return multi or f.pdur == 0
    if name == 'SepticStrike':
        return f.pdur >= 3
    if name == 'FanTheFlames':
        return any(e.bdur > 0 for e in g.alive())
    if name == 'Meltdown':
        return f.burn >= 5
    if name == 'Kindle':
        return f.burn < 8 or 'Inferno' not in installed_names
    # What the program will have built by the time a detonator runs: setup executes first.
    pending = (3 * installed_names.count('Ignite') + 2 * installed_names.count('ShortCircuit'))
    if name == 'FireDetonation':
        return f.spark + pending >= 8
    if name == 'ChainDetonation':
        return multi and (sum(e.spark for e in g.alive()) + pending + 2 * installed_names.count('StaticCharge')) >= 10
    if name == 'StaticCharge':
        return multi and rem >= 2
    if name == 'Ignite':
        return rem >= 2 or f.spark < 6
    if name in ('Inferno', 'Terminate', 'HeavyStrike', 'ChainLightning') and rem < 2:
        return False
    if name == 'Barrage':
        return f.mark >= 2
    if name == 'Dragnet':
        return True
    if name == 'Hunter':
        return rem >= 2
    return True


def choose_plays(hand, g, program, rem, energy, priority, multi, vuln_deck):
    plays = []
    installed = [c['name'] for c in program]
    hand = list(hand)
    while True:
        picked = None
        for name in priority:
            if name not in hand:
                continue
            cost = CARDS[name][0]
            if cost > energy:
                continue
            if not should_play(name, g, installed, rem, multi):
                continue
            picked = name
            break
        if picked is None:
            # Energy that would otherwise go unused: fall back to anything affordable.
            # Only plain damage cards - never a payoff with nothing to pay off, or a setup card that
            # the policy already decided was pointless.
            for name in hand:
                if name in FILLERS and CARDS[name][0] <= energy and CARDS[name][0] > 0 and not (CARDS[name][0] >= 2 and rem < 2):
                    picked = name
                    break
        if picked is None:
            break
        hand.remove(picked)
        energy -= CARDS[picked][0]
        plays.append(picked)
        installed.append(picked)
        # Free cards are always worth playing; stop only when nothing is left.
    return plays, energy


def program_value(g_template, program, rem_turns):
    """Damage the given program deals over the rest of the cycle with no further plays."""
    g = copy.deepcopy(g_template)
    prog = copy.deepcopy(program)
    for _ in range(rem_turns):
        run_program(g, prog)
        g.end_of_turn()
        prog = [c for c in prog if CARDS[c['name']][1]]
    return g.dealt


def play_game(cards, priority, hps, rng, use_vulnerable):
    g = Game(hps)
    multi = len(hps) > 1
    draw_pile = list(cards)
    rng.shuffle(draw_pile)
    discard, hand, program = [], [], []
    per_turn = []
    kill_turn = None
    for t in range(1, MAX_TURNS + 1):
        cyc = (t - 1) % CYCLE
        rem = CYCLE - cyc
        if cyc == 0 and t > 1:
            discard += [c['name'] for c in program]
            program = []
        for _ in range(DRAW):
            if not draw_pile:
                draw_pile, discard = discard, []
                rng.shuffle(draw_pile)
            if draw_pile:
                hand.append(draw_pile.pop())

        energy = ENERGY
        plays, left = choose_plays(hand, g, program, rem, energy, priority, multi, use_vulnerable)
        if use_vulnerable and 'Vulnerable' in hand:
            # Compare this turn's best plan against one that spends 1 energy on Vulnerable first,
            # by simulating the rest of the cycle with the cards as they would sit in the program.
            def build(pl, with_vuln):
                prog = copy.deepcopy(program)
                for n in pl:
                    prog.append({'name': n, 'n': 0})
                if with_vuln:
                    prog.append({'name': 'Vulnerable', 'n': 0})
                return prog
            plays_v, _ = choose_plays([h for h in hand if h != 'Vulnerable'], g, program, rem, energy - 1, priority, multi, False)
            base_v = program_value(g, build(plays, False), rem)
            vuln_v = program_value(g, build(plays_v, True), rem)
            if vuln_v > base_v:
                plays = plays_v + ['Vulnerable']
        nonpersistent = []
        for n in plays:
            hand.remove(n)
            c = {'name': n, 'n': 0}
            if CARDS[n][1]:
                program.append(c)
            else:
                nonpersistent.append(c)
        run_program(g, program + nonpersistent)
        g.end_of_turn()
        discard += [c['name'] for c in nonpersistent]
        discard += hand
        hand = []
        per_turn.append(g.dealt)
        if not g.alive():
            kill_turn = t
            break
    return per_turn, kill_turn


def evaluate(cards, key, hps, runs=300, use_vulnerable=False, seed=1):
    rng = random.Random(seed)
    kills, at3, at6 = [], [], []
    for _ in range(runs):
        per_turn, kill = play_game(cards, PRIORITY[key], hps, rng, use_vulnerable)
        kills.append(kill if kill is not None else MAX_TURNS + 1)
        at3.append(per_turn[2] if len(per_turn) > 2 else per_turn[-1])
        at6.append(per_turn[5] if len(per_turn) > 5 else per_turn[-1])
    return statistics.median(kills), statistics.mean(at3), statistics.mean(at6), sorted(kills)[int(runs * 0.9)]


ARCHETYPES = [
    ('Pure Physical', physical, 'physical', ['Strike', 'Cleave', 'Jab']),
    ('Pure Poison', poison, 'poison', ['Strike', 'Strike', 'HeavyStrike']),
    ('Pure Burn', burn, 'burn', ['Strike', 'Strike', 'HeavyStrike']),
    ('Pure Spark', spark, 'spark', ['Strike', 'HeavyStrike', 'HeavyStrike']),
    ('Pure Mark', mark, 'mark', ['Jab', 'Cleave', 'Dragnet']),
    ('Mixed', mixed, 'mixed', ['Strike', 'HeavyStrike', 'Mark']),
]

SCENARIOS = [
    ('Boss (500 HP)', [500]),
    ('Mini-boss (200 HP)', [200]),
    ('Pack (3 x 80 HP)', [80, 80, 80]),
]


def main():
    runs = int(sys.argv[1]) if len(sys.argv) > 1 else 300
    for label, hps in SCENARIOS:
        print('\n== %s ==' % label)
        print('%-22s %10s %10s %10s %10s' % ('deck', 'kill (med)', 'kill (p90)', 'dmg @T3', 'dmg @T6'))
        for name, maker, key, swap in ARCHETYPES:
            for vuln in (False, True):
                cards = maker()
                if vuln:
                    cards = with_vulnerable(cards, swap)
                med, a3, a6, p90 = evaluate(cards, key, hps, runs, use_vulnerable=vuln)
                tag = name + (' + Vuln' if vuln else '')
                print('%-22s %10.1f %10d %10.0f %10.0f' % (tag, med, p90, a3, a6))


if __name__ == '__main__':
    main()

"""Builds the card-name fonts in demo/assets from the bitmaps below: cardpixel.ttf (every row),
cardpixel-top.ttf and cardpixel-bottom.ttf (the rows above / below SPLIT_ROW).

The old hand-drawn card names were two-toned, switching colour where the card's light band met its
dark one. A font only has one colour, so the card draws the name twice - the top font in the light
colour, the bottom font in the dark one - and the halves line up because both fonts share metrics.

Run from the repo root:  python tools/build_cardpixel_font.py   (needs fonttools)

Each glyph is (first_row, [row strings]). Rows are numbered like the old hand-drawn card strips:
row 2 is the top of a capital, row 4 the top of a lowercase letter, row 7 the last row above the
baseline, row 8 the first descender row. A '#' is one design pixel.

Glyphs under EXTRACTED were lifted from the hand-drawn card sprites in assets/ui.png (as they
were before the card templates replaced them); DRAWN ones did not exist there and were drawn to
match. Edit either and rebuild.

Metrics: one design pixel is 100 units, em = 9 pixels (7 above the baseline, 2 below). GDI sizes a
font by its cell height, so asking for size 18 gives exactly 2 screen pixels per design pixel -
use multiples of 9 to stay crisp.
"""
import os
from fontTools.fontBuilder import FontBuilder
from fontTools.pens.ttGlyphPen import TTGlyphPen

PX = 100
ASCENT_ROWS = 7
DESCENT_ROWS = 2
BASELINE_ROW = 8   # row index just below the baseline
GAP = 1            # blank design pixels after every glyph
SPLIT_ROW = 5      # first row of the bottom half: rows 2-4 are the light band, 5+ the dark one

EXTRACTED = {
    'A': (2, ['.#.', '#.#', '###', '#.#', '#.#', '#.#']),
    'B': (2, ['##.', '#.#', '##.', '#.#', '#.#', '##.']),
    'C': (2, ['.#.', '#.#', '#..', '#..', '#.#', '.#.']),
    'E': (2, ['###', '#..', '##.', '#..', '#..', '###']),
    'H': (2, ['#.#', '#.#', '###', '#.#', '#.#', '#.#']),
    'I': (2, ['###', '.#.', '.#.', '.#.', '.#.', '###']),
    'J': (2, ['.##', '..#', '..#', '..#', '..#', '##.']),
    'L': (2, ['#..', '#..', '#..', '#..', '#..', '###']),
    'M': (2, ['##.#.', '#.#.#', '#...#', '#...#', '#...#', '#...#']),
    'O': (2, ['.#.', '#.#', '#.#', '#.#', '#.#', '.#.']),
    'P': (2, ['##.', '#.#', '##.', '#..', '#..', '#..']),
    'R': (2, ['##.', '#.#', '##.', '#.#', '#.#', '#.#']),
    'S': (2, ['.##', '#..', '.#.', '..#', '#.#', '.#.']),
    'T': (2, ['###', '.#.', '.#.', '.#.', '.#.', '.#.']),
    'V': (2, ['#.#', '#.#', '#.#', '#.#', '#.#', '.#.']),
    'W': (2, ['#...#', '#...#', '#...#', '#.#.#', '#.#.#', '.#.#.']),
    'a': (4, ['.##', '#.#', '#.#', '.##']),
    'b': (2, ['#..', '#..', '##.', '#.#', '#.#', '##.']),
    'c': (4, ['.##', '#..', '#..', '.##']),
    'd': (2, ['..#', '..#', '.##', '#.#', '#.#', '.##']),
    'e': (4, ['.#.', '#.#', '##.', '.##']),
    'g': (4, ['###', '#.#', '.##', '..#', '.#.']),
    'h': (2, ['#..', '#..', '##.', '#.#', '#.#', '#.#']),
    'i': (2, ['#', '.', '#', '#', '#', '#']),
    'k': (2, ['#..', '#..', '#.#', '##.', '#.#', '#.#']),
    'l': (2, ['#.', '#.', '#.', '#.', '#.', '.#']),
    'm': (4, ['##.#.', '#.#.#', '#.#.#', '#.#.#']),
    'n': (4, ['##.', '#.#', '#.#', '#.#']),
    'o': (4, ['.#.', '#.#', '#.#', '.#.']),
    'p': (4, ['##.', '#.#', '##.', '#..', '#..']),
    'r': (4, ['#.#', '##.', '#..', '#..']),
    's': (4, ['.#', '#.', '.#', '#.']),
    't': (4, ['#.', '##', '#.', '.#']),
    'u': (4, ['#.#', '#.#', '#.#', '.##']),
    'v': (4, ['#.#', '#.#', '#.#', '.#.']),
    'x': (4, ['#.#', '.#.', '#.#', '#.#']),
    'y': (4, ['#.#', '#.#', '.##', '..#', '.#.']),
}

DRAWN = {
    # Capitals missing from the old sprites.
    'D': (2, ['##.', '#.#', '#.#', '#.#', '#.#', '##.']),
    'F': (2, ['###', '#..', '##.', '#..', '#..', '#..']),
    'G': (2, ['.##', '#..', '#..', '#.#', '#.#', '.##']),
    'K': (2, ['#.#', '#.#', '##.', '##.', '#.#', '#.#']),
    'N': (2, ['##.', '#.#', '#.#', '#.#', '#.#', '#.#']),
    'Q': (2, ['.#.', '#.#', '#.#', '#.#', '##.', '.##']),
    'U': (2, ['#.#', '#.#', '#.#', '#.#', '#.#', '###']),
    'X': (2, ['#.#', '#.#', '.#.', '.#.', '#.#', '#.#']),
    'Y': (2, ['#.#', '#.#', '#.#', '.#.', '.#.', '.#.']),
    'Z': (2, ['###', '..#', '.#.', '.#.', '#..', '###']),
    # Lowercase missing from the old sprites.
    'f': (2, ['.#', '#.', '##', '#.', '#.', '#.']),
    'j': (2, ['.#', '..', '.#', '.#', '.#', '.#', '#.']),
    'q': (4, ['.##', '#.#', '.##', '..#', '..#']),
    'w': (4, ['#...#', '#.#.#', '#.#.#', '.#.#.']),
    'z': (4, ['###', '..#', '.#.', '###']),
    # Digits: 0-4 are the cost digits from the old sprites (fill only, the outline was drawn
    # around them); 5-9 did not exist.
    '0': (2, ['###', '#.#', '#.#', '#.#', '#.#', '###']),
    '1': (2, ['.#.', '##.', '.#.', '.#.', '.#.', '###']),
    '2': (2, ['.#.', '#.#', '..#', '.#.', '#..', '###']),
    '3': (2, ['##.', '..#', '.#.', '..#', '#.#', '.#.']),
    '4': (2, ['#.#', '#.#', '###', '..#', '..#', '..#']),
    '5': (2, ['###', '#..', '##.', '..#', '#.#', '.#.']),
    '6': (2, ['.##', '#..', '##.', '#.#', '#.#', '.#.']),
    '7': (2, ['###', '..#', '..#', '.#.', '.#.', '.#.']),
    '8': (2, ['.#.', '#.#', '.#.', '#.#', '#.#', '.#.']),
    '9': (2, ['.#.', '#.#', '#.#', '.##', '..#', '##.']),
    # Punctuation.
    '(': (2, ['.#', '#.', '#.', '#.', '#.', '.#']),
    ')': (2, ['#.', '.#', '.#', '.#', '.#', '#.']),
    '_': (8, ['###']),
    '-': (5, ['###']),
    '.': (7, ['#']),
    ',': (7, ['.#', '#.']),
    ':': (4, ['#', '.', '.', '#']),
    '!': (2, ['#', '#', '#', '#', '.', '#']),
    '?': (2, ['##.', '..#', '.#.', '...', '.#.']),
    "'": (2, ['#', '#']),
    '/': (2, ['..#', '..#', '.#.', '.#.', '#..', '#..']),
    '+': (4, ['.#.', '###', '.#.']),
    ' ': (2, ['..']),
}

GLYPHS = {**EXTRACTED, **DRAWN}


def glyph_for(rows_def, keep):
    first, rows = rows_def
    pen = TTGlyphPen(None)
    width = max(len(r) for r in rows)
    for i, row in enumerate(rows):
        r = first + i
        if not keep(r):
            continue
        y0 = (BASELINE_ROW - 1 - r) * PX
        x = 0
        while x < len(row):
            if row[x] != '#':
                x += 1
                continue
            start = x
            while x < len(row) and row[x] == '#':
                x += 1
            # Clockwise square, one run of pixels per rectangle.
            pen.moveTo((start * PX, y0))
            pen.lineTo((start * PX, y0 + PX))
            pen.lineTo((x * PX, y0 + PX))
            pen.lineTo((x * PX, y0))
            pen.closePath()
    return pen.glyph(), (width + GAP) * PX


def build(path, family, keep):
    names = ['.notdef'] + ['uni%04X' % ord(c) for c in sorted(GLYPHS)]
    cmap = {ord(c): 'uni%04X' % ord(c) for c in GLYPHS}
    glyphs = {'.notdef': TTGlyphPen(None).glyph()}
    metrics = {'.notdef': (4 * PX, 0)}
    for c, d in GLYPHS.items():
        name = 'uni%04X' % ord(c)
        glyph, advance = glyph_for(d, keep)
        glyphs[name] = glyph
        metrics[name] = (advance, 0)

    em = (ASCENT_ROWS + DESCENT_ROWS) * PX
    fb = FontBuilder(em, isTTF=True)
    fb.setupGlyphOrder(names)
    fb.setupCharacterMap(cmap)
    fb.setupGlyf(glyphs)
    glyf = fb.font['glyf']
    hmtx = {}
    for n in names:
        glyph = glyf[n]
        glyph.recalcBounds(glyf)
        hmtx[n] = (metrics[n][0], getattr(glyph, 'xMin', 0) if glyph.numberOfContours else 0)
    fb.setupHorizontalMetrics(hmtx)
    fb.setupHorizontalHeader(ascent=ASCENT_ROWS * PX, descent=-DESCENT_ROWS * PX)
    # Windows refuses a font without the full set of name records, not just family and style.
    fb.setupNameTable({
        'familyName': family,
        'styleName': 'Regular',
        'uniqueFontIdentifier': family + '-Regular;1.0',
        'fullName': family + ' Regular',
        'psName': family + '-Regular',
        'version': 'Version 1.0',
    })
    fb.setupOS2(sTypoAscender=ASCENT_ROWS * PX, sTypoDescender=-DESCENT_ROWS * PX, sTypoLineGap=0,
                usWinAscent=ASCENT_ROWS * PX, usWinDescent=DESCENT_ROWS * PX, fsType=0,
                achVendID='NONE', fsSelection=0x40, usWeightClass=400, xAvgCharWidth=4 * PX)
    fb.setupPost()
    fb.save(path)


if __name__ == '__main__':
    assets = os.path.join(os.path.dirname(__file__), '..', 'demo', 'assets')
    for filename, family, keep in [
        ('cardpixel.ttf', 'CardPixel', lambda r: True),
        ('cardpixel-top.ttf', 'CardPixelTop', lambda r: r < SPLIT_ROW),
        ('cardpixel-bottom.ttf', 'CardPixelBottom', lambda r: r >= SPLIT_ROW),
    ]:
        out = os.path.normpath(os.path.join(assets, filename))
        build(out, family, keep)
        print('wrote', out, 'with', len(GLYPHS), 'glyphs')

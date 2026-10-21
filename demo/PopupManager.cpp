#include "pch.h"
#include "PopupManager.h"
#include "DX9GFInputManager.h"
#include <algorithm>
#include <cmath>
#include "TextIconButton.h"
#include "Game.h"]
namespace Demo {
    // Horizontal breathing room between a popup button's label and its edge. Must match the
    // padding handed to TextIconButton::SetAutoResize so Show()'s width estimate agrees with the
    // size the button settles at.
    static constexpr float BTN_TEXT_PAD_X = 16.0f;

    // Widest a message line may get before it is wrapped, and the gap between wrapped lines.
    static constexpr float MAX_MSG_WIDTH = 800.0f;
    static constexpr float MSG_LINE_SPACING = 6.0f;

    // Greedy word-wrap of `text` (honouring newlines) to `maxWidth` using the sprite's metrics.
    static std::vector<std::wstring> WrapMessage(DX9GF::FontSprite* fs, const std::wstring& text, float maxWidth) {
        std::vector<std::wstring> lines;
        size_t pos = 0;
        while (pos <= text.size()) {
            size_t nl = text.find(L'\n', pos);
            std::wstring para = text.substr(pos, nl == std::wstring::npos ? std::wstring::npos : nl - pos);
            std::wstring line;
            size_t p = 0;
            while (p < para.size()) {
                size_t sp = para.find(L' ', p);
                std::wstring word = para.substr(p, sp == std::wstring::npos ? std::wstring::npos : sp - p);
                std::wstring cand = line.empty() ? word : line + L" " + word;
                fs->SetText(cand);
                if (!line.empty() && fs->GetWidth() > maxWidth) {
                    lines.push_back(line);
                    line = word;
                }
                else {
                    line = cand;
                }
                if (sp == std::wstring::npos) break;
                p = sp + 1;
            }
            lines.push_back(line);
            if (nl == std::wstring::npos) break;
            pos = nl + 1;
        }
        return lines;
    }

    PopupManager* PopupManager::instance = nullptr;

    PopupManager::PopupManager() {}

    PopupManager* PopupManager::GetInstance() {
        if (!instance) {
            instance = new PopupManager();
        }
        return instance;
    }

    void PopupManager::Init(Game* game, std::shared_ptr<DX9GF::Texture> borderTex, std::shared_ptr<DX9GF::Texture> uiTex, std::shared_ptr<DX9GF::Font> popupFont) {
        this->game = game;
        this->font = popupFont;
        this->fontSprite = std::make_shared<DX9GF::FontSprite>(this->font.get());
        this->internalTm = std::make_shared<DX9GF::TransformManager>();

        this->borderSheetTex = borderTex;
        this->uiSheetTex = uiTex;

        RegisterStyle("basic_blackwhite", { {0, 0, 32, 32},     2, 2, 2, 2, 0xFF000000, 0xFF000000, false, 0x00000000 });
        RegisterStyle("basic_blackgrey", { {32, 0, 64, 32},    2, 2, 2, 2, 0xFFFFFFFF, 0xFFFFFFFF, true,  0xFF000000 });
        RegisterStyle("basic_whitegrey", { {64, 0, 96, 32},    2, 2, 2, 2, 0xFF000000, 0xFF000000, false, 0x00000000 });
        RegisterStyle("basic_ghost", { {96, 0, 127, 32},   2, 2, 2, 2, 0xFF000000, 0xFF000000, true,  0xFFFFFFFF });
        RegisterStyle("basic_blueyellow", { {0, 64, 32, 96},    2, 2, 2, 2, 0xFFFFFFFF, 0xFFFFFFFF, false, 0x00000000 });
        RegisterStyle("basic_bluepurple", { {32, 64, 64, 96},   2, 2, 2, 2, 0xFFFFFFFF, 0xFFFFFFFF, false, 0x00000000 });
        RegisterStyle("basic_contrastblue", { {64, 64, 96, 96},   2, 2, 2, 2, 0xFFFFFFFF, 0xFFFFFFFF, true,  0xFF000000 });
        RegisterStyle("basic_contrastred", { {96, 64, 128, 96},  2, 2, 2, 2, 0xFFFFFFFF, 0xFFFFFFFF, false, 0x00000000 });
        RegisterStyle("stepped_white", { {32, 128, 64, 160},   6, 6, 6, 6, 0xFF000000, 0xFF000000, false, 0x00000000 });
        RegisterStyle("stepped_red", { {64, 128, 96, 160},   6, 6, 6, 6, 0xFFFFFFFF, 0xFFFFFFFF, true,  0xFF000000 });
        RegisterStyle("stepped_blue", { {96, 128, 128, 160},  6, 6, 6, 6, 0xFFFFFFFF, 0xFFFFFFFF, true,  0xFF000000 });
        RegisterStyle("stepped_gold", { {128, 128, 160, 160}, 6, 6, 6, 6, 0xFF000000, 0xFF000000, true,  0xFFFFFFFF });

        btnRectsSmall = { {0, 0, 16, 16}, {0, 16, 16, 32}, {0, 32, 16, 48} };
        btnRectsMed = { {16, 0, 48, 16}, {16, 16, 48, 32}, {16, 32, 48, 48} };
        btnRectsLarge = { {48, 0, 96, 16}, {48, 16, 96, 32}, {48, 32, 96, 48} };
    }

    void PopupManager::RegisterStyle(const std::string& styleName, const PopupStyle& style) {
        styles[styleName] = style;
    }

    void PopupManager::Show(const std::string& styleName, const std::wstring& title, const std::wstring& message,
        const std::vector<std::pair<std::wstring, std::function<void()>>>& buttons)
    {
        // Only ShowWithSideIcon asks for an icon column; every other popup gets none.
        const float requestedSideIcon = pendingSideIconSize;
        pendingSideIconSize = 0.0f;
        if (styles.find(styleName) == styles.end() || !this->uiCamera) return;
        this->sideIconSize = requestedSideIcon;
        const float sideW = sideIconSize > 0.0f ? sideIconSize + 20.0f : 0.0f;

        this->currentStyle = styleName;
        this->currentTitle = title;
        this->currentMessage = message;
        this->isActive = true;
        this->iconSprite = nullptr;
        this->iconTexture = nullptr;
        this->iconSize = 0.0f;
        this->activeButtons.clear();
        this->internalTm = std::make_shared<DX9GF::TransformManager>();

        PopupStyle& style = styles[styleName];

        fontSprite->SetText(std::wstring(currentTitle));
        float titleW = static_cast<float>(fontSprite->GetWidth());
        float titleH = static_cast<float>(fontSprite->GetHeight());

        fontSprite->SetText(std::wstring(currentMessage));
        float msgW = static_cast<float>(fontSprite->GetWidth());
        float msgH = static_cast<float>(fontSprite->GetHeight());
        this->messageLines.clear();
        const float maxMsgW = std::min(MAX_MSG_WIDTH, static_cast<float>(game->GetVirtualWidth()) - 160.0f) - sideW;
        if (msgW > maxMsgW) {
            this->messageLines = WrapMessage(fontSprite.get(), currentMessage, maxMsgW);
            msgW = 0.0f;
            for (const auto& line : messageLines) {
                fontSprite->SetText(line);
                msgW = std::max(msgW, static_cast<float>(fontSprite->GetWidth()));
            }
            this->messageLineH = msgH;
            msgH = (msgH + MSG_LINE_SPACING) * messageLines.size() - MSG_LINE_SPACING;
        }

        this->messageW = msgW;
        this->messageH = msgH;

        float scale = 2.0f; //Change this value to adjust the button size
        float baseBtnH = static_cast<float>(btnRectsMed[0].bottom - btnRectsMed[0].top);
        float btnH = baseBtnH * scale;
        // Smallest a button is allowed to be; longer labels widen it past this instead of spilling.
        float minBtnW = static_cast<float>(btnRectsMed[0].right - btnRectsMed[0].left) * scale;

        float spacing = 20.0f;
        // Each button fits its own label (measured with the popup font, same font the button draws).
        std::vector<float> btnWidths;
        float totalBtnW = 0.0f;
        for (const auto& btnData : buttons) {
            fontSprite->SetText(std::wstring(btnData.first));
            float w = std::floor(std::max(fontSprite->GetWidth() + BTN_TEXT_PAD_X * 2.0f, minBtnW) + 0.5f);
            btnWidths.push_back(w);
            totalBtnW += w;
        }
        totalBtnW += std::max(0, (int)buttons.size() - 1) * spacing;

        float paddingX = 80.0f;
        float maxContentW = std::max({ titleW, msgW + sideW, totalBtnW });
        this->popupWidth = std::max(maxContentW + paddingX, 300.0f);

        float paddingTop = 25.0f;
        float paddingMid = 20.0f;
        float paddingBot = 25.0f;

        this->popupHeight = paddingTop + titleH + paddingMid + std::max(msgH, sideIconSize) + paddingMid + btnH + paddingBot;

        this->backgroundSprite = std::make_shared<DX9GF::NineSliceSprite>(
            borderSheetTex.get(), style.bgSrcRect,
            style.marginL, style.marginT, style.marginR, style.marginB
        );
        this->backgroundSprite->SetTargetSize(popupWidth, popupHeight);
        this->backgroundSprite->SetScale(scale);

        for (size_t i = 0; i < buttons.size(); ++i) {
            const auto& btnData = buttons[i];

            auto textBtn = std::make_shared<TextIconButton>(internalTm, 0, 0, (int)btnWidths[i], (int)btnH, uiSheetTex, this->font.get(), btnData.first, 3);
            textBtn->SetSpriteRects(btnRectsMed);
            textBtn->SetSpriteScale(scale, scale);
            // Slice the frame so a widened button stretches its centre instead of smearing its caps.
            textBtn->SetSliceMargins(6, 6);
            textBtn->SetAutoResize(true, BTN_TEXT_PAD_X);
            textBtn->SetMinWidth(minBtnW);
            textBtn->SetTextColor(0xFF111111);
            textBtn->Init(this->uiCamera);

            auto popupBtn = std::make_shared<PopupDynamicButton>(textBtn, btnData.first, btnData.second);

            auto cb = btnData.second;
            textBtn->SetOnReleaseLeft([this, cb](DX9GF::ITrigger* t) {
                if (cb) cb();
                this->Close();
                });

            activeButtons.push_back(popupBtn);
        }

        LayoutButtons();
        keyboardNavigator.Reset();
    }

    void PopupManager::ShowWithIcon(const std::string& styleName, const std::wstring& title, const std::wstring& message,
        const std::vector<std::pair<std::wstring, std::function<void()>>>& buttons,
        std::shared_ptr<DX9GF::Texture> iconTex, const std::vector<RECT>& iconFrames, float iconSize)
    {
        Show(styleName, title, message, buttons);
        if (!isActive || !iconTex || iconFrames.empty()) return;

        this->iconTexture = iconTex;
        this->iconSprite = std::make_shared<DX9GF::AnimatedSprite>(iconTex.get(), iconFrames);
        this->iconSprite->SetFrameRate(12);
        this->iconSize = iconSize;
        float frameH = static_cast<float>(iconFrames[0].bottom - iconFrames[0].top);
        float s = frameH > 0.0f ? iconSize / frameH : 1.0f;
        this->iconSprite->SetScale(s, s);

        // Make room for the icon (plus its spacing) and let the buttons follow the new bottom edge.
        popupHeight += iconSize + 20.0f;
        backgroundSprite->SetTargetSize(popupWidth, popupHeight);
        LayoutButtons();
    }


    void PopupManager::ShowWithSideIcon(const std::string& styleName, const std::wstring& title, const std::wstring& message,
        const std::vector<std::pair<std::wstring, std::function<void()>>>& buttons,
        std::shared_ptr<DX9GF::Texture> iconTex, const std::vector<RECT>& iconFrames, float iconSize)
    {
        // Show lays the popup out with the icon column already reserved, so no resize is needed here.
        if (iconTex && !iconFrames.empty()) pendingSideIconSize = iconSize;
        Show(styleName, title, message, buttons);
        if (!isActive || sideIconSize <= 0.0f) return;

        this->iconTexture = iconTex;
        this->iconSprite = std::make_shared<DX9GF::AnimatedSprite>(iconTex.get(), iconFrames);
        this->iconSprite->SetFrameRate(12);
        this->iconSize = sideIconSize;
        float frameH = static_cast<float>(iconFrames[0].bottom - iconFrames[0].top);
        float s = frameH > 0.0f ? sideIconSize / frameH : 1.0f;
        this->iconSprite->SetScale(s, s);
    }

    std::vector<KeyboardNavigator::Candidate> PopupManager::CollectKeyboardCandidates() {
        std::vector<KeyboardNavigator::Candidate> candidates;
        for (auto& popupBtn : activeButtons) {
            auto btn = popupBtn->backgroundBtn;
            if (!btn) {
                continue;
            }
            candidates.push_back({
                btn,
                btn->GetWorldX(),
                btn->GetWorldY(),
                (float)btn->GetWidth(),
                (float)btn->GetHeight(),
                [btn]() { btn->Activate(); }
                });
        }
        return candidates;
    }

    void PopupManager::LayoutButtons() {
        if (!this->uiCamera) return;

        popupX = -popupWidth / 2.0f;
        popupY = -popupHeight / 2.0f;

        if (activeButtons.empty()) return;

        float btnH = activeButtons[0]->backgroundBtn->GetHeight();
        float spacing = 20.0f;

        // Buttons can each be a different width (they grow to fit their label), so sum the actual
        // widths rather than assuming a uniform size.
        float totalWidth = std::max(0, (int)activeButtons.size() - 1) * spacing;
        for (auto& popupBtn : activeButtons) totalWidth += popupBtn->backgroundBtn->GetWidth();

        float startX = popupX + (popupWidth - totalWidth) / 2.0f;
        float startY = popupY + popupHeight - btnH - 25.0f;

        float x = startX;
        for (auto& popupBtn : activeButtons) {
            popupBtn->backgroundBtn->SetLocalPosition(x, startY);
            x += popupBtn->backgroundBtn->GetWidth() + spacing;
        }
        internalTm->RebuildHierarchy();
    }

    void PopupManager::Close() {
        this->isActive = false;
        keyboardNavigator.Reset();
    }

    void PopupManager::Update(unsigned long long deltaTime, DX9GF::Camera* uiCamera) {
        if (!isActive) return;

        internalTm->UpdateAll();

        for (size_t i = 0; i < activeButtons.size(); ++i) {
            activeButtons[i]->backgroundBtn->Update(deltaTime);

            if (!isActive) break;
        }
        // Re-run the layout each frame: a button only settles on its label-fitted width after its
        // first Update, and its position has to follow.
        if (isActive) LayoutButtons();
        if (isActive) {
            auto inpMan = DX9GF::InputManager::GetInstance();
            for (auto& popupBtn : activeButtons) {
                // Any label carrying a "(Y)" / "(N)" hotkey hint, not just "Yes(Y)" / "No(N)".
                bool isYesKey = (popupBtn->label.find(L"(Y)") != std::wstring::npos && inpMan->KeyPress(DIK_Y));
                bool isNoKey = (popupBtn->label.find(L"(N)") != std::wstring::npos && inpMan->KeyPress(DIK_N));
                if (isYesKey || isNoKey) {
                    if (popupBtn->onClick) popupBtn->onClick();
                    Close();
                    break;
                }
            }
        }
        if (isActive) {
            keyboardNavigator.Update(deltaTime, CollectKeyboardCandidates());
        }
    }

    void PopupManager::DrawUI(unsigned long long deltaTime, DX9GF::Camera* uiCamera) {
        if (!isActive || !game) return;

        float sw = static_cast<float>(game->GetVirtualWidth());
        float sh = static_cast<float>(game->GetVirtualHeight());
        auto gd = game->GetGraphicsDevice();

        gd->SetAlphaBlending(true);
        gd->DrawRectangle(*uiCamera, -sw / 2.0f, -sh / 2.0f, (float)sw, (float)sh, 0x88000000, true);
        gd->SetAlphaBlending(false);

        backgroundSprite->Begin();
        backgroundSprite->SetPosition(popupX, popupY);
        backgroundSprite->Draw(*uiCamera, deltaTime);
        backgroundSprite->End();

        PopupStyle& style = styles[currentStyle];

        fontSprite->Begin();
        fontSprite->SetText(std::wstring(currentTitle));
        fontSprite->SetColor(style.titleColor);
        fontSprite->SetOutline(style.hasOutline, style.outlineColor, 2.0f);

        float titleW = fontSprite->GetWidth();
        float titleH = fontSprite->GetHeight();
        fontSprite->SetPosition(popupX + (popupWidth - titleW) / 2.0f, popupY + 25.0f);
        fontSprite->Draw(*uiCamera, deltaTime);

        fontSprite->SetText(std::wstring(currentMessage));
        fontSprite->SetColor(style.msgColor);

        const float contentTop = popupY + 25.0f + titleH + 20.0f;
        // Side icon: the icon and the message form one centred row, each centred vertically in it.
        // Otherwise the icon (if any) sits above the message, as before.
        const float sideW = sideIconSize > 0.0f ? sideIconSize + 20.0f : 0.0f;
        const float rowH = std::max(messageH, sideIconSize);
        const float rowX = popupX + (popupWidth - (sideW + messageW)) / 2.0f;
        const float iconBlock = (iconSprite && sideW <= 0.0f) ? iconSize + 20.0f : 0.0f;
        const float msgTop = sideW > 0.0f ? contentTop + (rowH - messageH) / 2.0f : contentTop + iconBlock;
        auto lineX = [&](float w) {
            return sideW > 0.0f ? rowX + sideW + (messageW - w) / 2.0f : popupX + (popupWidth - w) / 2.0f;
            };

        if (messageLines.empty()) {
            float msgW = fontSprite->GetWidth();
            fontSprite->SetPosition(lineX(msgW), msgTop);
            fontSprite->Draw(*uiCamera, deltaTime);
        }
        else {
            float y = msgTop;
            for (const auto& line : messageLines) {
                fontSprite->SetText(line);
                fontSprite->SetPosition(lineX(static_cast<float>(fontSprite->GetWidth())), y);
                fontSprite->Draw(*uiCamera, deltaTime);
                y += messageLineH + MSG_LINE_SPACING;
            }
        }
        fontSprite->End();

        if (iconSprite) {
            const float iconX = sideW > 0.0f ? rowX : popupX + (popupWidth - iconSize) / 2.0f;
            const float iconY = sideW > 0.0f ? contentTop + (rowH - iconSize) / 2.0f : contentTop;
            gd->SetAlphaBlending(true);
            iconSprite->Begin();
            iconSprite->SetPosition(iconX, iconY);
            iconSprite->Draw(*uiCamera, deltaTime);
            iconSprite->End();
        }

        gd->SetAlphaBlending(true);
        for (auto& btn : activeButtons) {
            btn->backgroundBtn->Draw(gd, deltaTime);
        }

        keyboardNavigator.Draw(gd, *uiCamera, CollectKeyboardCandidates());
    }
}
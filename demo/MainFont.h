#pragma once

namespace Demo {
	// The GDI family name embedded in assets/135openpixel-v2-3.ttf, used as the
	// main UI font everywhere Vietnamese text needs to render correctly.
	constexpr const wchar_t* kMainFontName = L"135OpenPixel v2.3";

	// The font is drawn on a 13-pixel cell (10 ascent + 3 descent), and D3DXCreateFont's
	// height is the cell height. Any size that is not a multiple of 13 makes GDI round font
	// pixels to a mix of 1/2/3 screen pixels, so strokes come out uneven and thin.
	constexpr int kMainFontCell = 13;
	constexpr int kSmallFontSize = kMainFontCell;      // 1x
	constexpr int kMainFontSize = kMainFontCell * 2;   // 2x
	constexpr int kLargeFontSize = kMainFontCell * 3;  // 3x
}

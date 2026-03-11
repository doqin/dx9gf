//#include "Debug.h"
//#include <string>
//
//FontDebug* DebugCourt::font;
//std::map<int, std::wstring> DebugCourt::coordinateCache;
//
//void DebugCourt::Init(DX9GF::GraphicsDevice* dev)
//{
//    if (!font) font = new FontDebug();
//    font->Init(dev);
//    for(int i = -1000; i <= 1000; i += 16)
//    {
//        coordinateCache[i] = std::to_wstring(i);
//	}
//}
//
//void DebugCourt::Release() {
//    if (font) {
//        delete font;
//        font = nullptr;
//    }
//}
//
//void DebugCourt::DrawAxisX(
//    DX9GF::GraphicsDevice* dev,
//    DX9GF::Camera& cam,
//    int startX,
//    int endX,
//    int step)
//{
//    font->Begin();
//    for (int x = startX - 16; x <= endX; x += step)
//    {
//        if (coordinateCache.find(x) == coordinateCache.end()) {
//            coordinateCache[x] = std::to_wstring(x);
//        }
//        font->Draw(cam, x, 0, std::to_wstring(x));
//    }
//    font->End();
//}
//
//void DebugCourt::DrawAxisY(
//    DX9GF::GraphicsDevice* dev,
//    DX9GF::Camera& cam,
//    int startY,
//    int endY,
//    int step)
//{
//    font->Begin();
//    for (int y = startY -16; y <= endY; y += step)
//    {
//        if (coordinateCache.find(y) == coordinateCache.end()) {
//            coordinateCache[y] = std::to_wstring(y);
//        }
//        font->Draw(cam, 0, y, std::to_wstring(y));
//    }    
//    font->End();
//}
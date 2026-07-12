#include "CanvasBuffer.h"

#include "Components/CanvasPanelSlot.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Misc/FileHelper.h"
#include "Modules/ModuleManager.h"


#undef UpdateResource


using namespace std;

UCanvasBuffer::UCanvasBuffer() {

}

UCanvasBuffer::~UCanvasBuffer() {

}

void UCanvasBuffer::InitCanvas(int width, int height) {
	canvas.Init(width, height);

	canvas.RegisterKey("space", FKeyCode(EKeys::SpaceBar));
	canvas.RegisterKey("up", FKeyCode(EKeys::Up));
	canvas.RegisterKey("down", FKeyCode(EKeys::Down));
	canvas.RegisterKey("left", FKeyCode(EKeys::Left));
	canvas.RegisterKey("right", FKeyCode(EKeys::Right));
	canvas.RegisterKey("enter", FKeyCode(EKeys::Enter));
	canvas.RegisterKey("escape", FKeyCode(EKeys::Escape));
	canvas.RegisterKey("r", FKeyCode(EKeys::R));
	canvas.RegisterKey("backspace", FKeyCode(EKeys::BackSpace));
	canvas.RegisterKey("tab", FKeyCode(EKeys::Tab));
}

FVector2D UCanvasBuffer::ApplyImage(UImage* image) {
	int width = canvas.GetWidth();
	int height = canvas.GetHeight();

	if (!image) return FVector2D(width, height);

	if (!tex || tex->GetSizeX() != width || tex->GetSizeY() != height) {
		tex = UTexture2D::CreateTransient(width, height);
		if (!tex) return FVector2D(width, height);
	}

	void* data = tex->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
	FMemory::Memcpy(data, canvas.GetData(), canvas.GetDataSize());
	tex->GetPlatformData()->Mips[0].BulkData.Unlock();

	tex->UpdateResource();
	image->SetBrushFromTexture(tex, true);
	return FVector2D(width, height);
}

void UCanvasBuffer::SetColor(uint8 r, uint8 g, uint8 b) {
	canvas.SetColor(r, g, b);
}

void UCanvasBuffer::SetAlpha(uint8 a) {
	canvas.SetAlpha(a);
}

void UCanvasBuffer::SetFontSize(int size) {
	canvas.SetFontSize(size);
}

void UCanvasBuffer::SetFontName(const FString& name) {
	canvas.SetFontName(string(TCHAR_TO_UTF8(*name)));
}

void UCanvasBuffer::SetFontStyle(bool bold, bool italic, bool underline) {
	canvas.SetFontStyle(bold, italic, underline);
}

void UCanvasBuffer::ClearBuffer() {
	canvas.ClearBuffer();
}

void UCanvasBuffer::PutPixel(int x, int y) {
	canvas.PutPixel(x, y);
}

void UCanvasBuffer::GetPixel(int x, int y, uint8& r, uint8& g, uint8& b, uint8& a) {
	canvas.GetPixel(x, y, r, g, b, a);
}

void UCanvasBuffer::PutLine(int x1, int y1, int x2, int y2) {
	canvas.PutLine(x1, y1, x2, y2);
}

void UCanvasBuffer::PutRect(int x1, int y1, int x2, int y2, bool fill) {
	canvas.PutRect(x1, y1, x2, y2, fill);
}

void UCanvasBuffer::PutTriangle(int x1, int y1, int x2, int y2, int x3, int y3, bool fill) {
	canvas.PutTriangle(x1, y1, x2, y2, x3, y3, fill);
}

void UCanvasBuffer::PutCircle(int x, int y, int radius, bool fill) {
	canvas.PutCircle(x, y, radius, fill);
}

void UCanvasBuffer::PutEllipse(int x, int y, int radiusX, int radiusY, bool fill) {
	canvas.PutEllipse(x, y, radiusX, radiusY, fill);
}

void UCanvasBuffer::PutImage(const FString& file, int x, int y, int width, int height) {
	TArray<uint8> fileData;
	if (!FFileHelper::LoadFileToArray(fileData, *file)) return;

	IImageWrapperModule& imageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>("ImageWrapper");
	EImageFormat imageFormat = imageWrapperModule.DetectImageFormat(fileData.GetData(), fileData.Num());
	if (imageFormat == EImageFormat::Invalid) return;

	TSharedPtr<IImageWrapper> wrapper = imageWrapperModule.CreateImageWrapper(imageFormat);
	if (!wrapper->SetCompressed(fileData.GetData(), fileData.Num())) return;

	TArray<uint8> raw;
	if (!wrapper->GetRaw(ERGBFormat::BGRA, 8, raw)) return;

	canvas.PutRawImage(raw.GetData(), wrapper->GetWidth(), wrapper->GetHeight(), x, y, width, height);
}

void UCanvasBuffer::PutString(const FString& text, int x, int y) {
	if (text.IsEmpty()) return;
	canvas.PutString(string(TCHAR_TO_UTF8(*text)), x, y);
}

int32 UCanvasBuffer::FKeyCode(FKey key) {
	return (int32)(GetTypeHash(key.GetFName()) & ~(uint32)0x8000);
}

void UCanvasBuffer::ReceiveKeyDown(FKey key) {
	canvas.PushKey(FKeyCode(key));
}

void UCanvasBuffer::ReceiveKeyUp(FKey key) {
	canvas.PushKey(FKeyCode(key) | 0x8000);
}

int32 UCanvasBuffer::BiosKey(int32 cmd) {
	return canvas.BiosKey(cmd);
}

void UCanvasBuffer::ClearKeyBuffer() {
	canvas.ClearKeyBuffer();
}

void UCanvasBuffer::ReceiveMouseMove(int32 x, int32 y) {
	canvas.SetMousePos(x, y);
}

void UCanvasBuffer::ReceiveMouseButtonDown(int32 button) {
	canvas.PushMouseButton(button, true);
}

void UCanvasBuffer::ReceiveMouseButtonUp(int32 button) {
	canvas.PushMouseButton(button, false);
}

int32 UCanvasBuffer::FMouseButton(FKey key) {
	if (key == EKeys::LeftMouseButton) return 0;
	if (key == EKeys::RightMouseButton) return 1;
	if (key == EKeys::MiddleMouseButton) return 2;
	return -1;
}

int32 UCanvasBuffer::BiosMouse(int32 cmd) {
	return canvas.BiosMouse(cmd);
}

void UCanvasBuffer::ClearMouseBuffer() {
	canvas.ClearMouseBuffer();
}

int32 UCanvasBuffer::GetMouseX() const {
	return canvas.MouseStatus().x;
}

int32 UCanvasBuffer::GetMouseY() const {
	return canvas.MouseStatus().y;
}

bool UCanvasBuffer::IsMouseButtonDown(int32 button) const {
	if (button < 0 || button > 2) return false;
	return canvas.MouseStatus().buttons[button];
}

Canvas* UCanvasBuffer::GetCanvas() {
	return &canvas;
}

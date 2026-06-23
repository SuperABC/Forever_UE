#pragma once

#include "CoreMinimal.h"
#include "Components/Image.h"

#include "player/canvas.h"
#include "player/puzzle.h"

#include "CanvasBuffer.generated.h"


UCLASS(Blueprintable, BlueprintType)
class FOREVER_API UCanvasBuffer : public UObject {
	GENERATED_BODY()

public:
	UCanvasBuffer();
	~UCanvasBuffer();

	UFUNCTION(BlueprintCallable, Category = "CanvasBuffer")
	void InitCanvas(int width, int height);
	UFUNCTION(BlueprintCallable, Category = "CanvasBuffer")
	FVector2D ApplyImage(UImage* image);

	UFUNCTION(BlueprintCallable, Category = "CanvasBuffer")
	void SetColor(uint8 r, uint8 g, uint8 b);
	UFUNCTION(BlueprintCallable, Category = "CanvasBuffer")
	void SetAlpha(uint8 a);
	UFUNCTION(BlueprintCallable, Category = "CanvasBuffer")
	void SetFontSize(int size);
	UFUNCTION(BlueprintCallable, Category = "CanvasBuffer")
	void SetFontName(const FString& name);
	UFUNCTION(BlueprintCallable, Category = "CanvasBuffer")
	void SetFontStyle(bool bold, bool italic, bool underline);
	UFUNCTION(BlueprintCallable, Category = "CanvasBuffer")
	void ClearBuffer();
	UFUNCTION(BlueprintCallable, Category = "CanvasBuffer")
	void PutPixel(int x, int y);
	UFUNCTION(BlueprintCallable, Category = "CanvasBuffer")
	void GetPixel(int x, int y, uint8& r, uint8& g, uint8& b, uint8& a);
	UFUNCTION(BlueprintCallable, Category = "CanvasBuffer")
	void PutLine(int x1, int y1, int x2, int y2);
	UFUNCTION(BlueprintCallable, Category = "CanvasBuffer")
	void PutRect(int x1, int y1, int x2, int y2, bool fill);
	UFUNCTION(BlueprintCallable, Category = "CanvasBuffer")
	void PutTriangle(int x1, int y1, int x2, int y2, int x3, int y3, bool fill);
	UFUNCTION(BlueprintCallable, Category = "CanvasBuffer")
	void PutCircle(int x, int y, int radius, bool fill);
	UFUNCTION(BlueprintCallable, Category = "CanvasBuffer")
	void PutEllipse(int x, int y, int radiusX, int radiusY, bool fill);
	UFUNCTION(BlueprintCallable, Category = "CanvasBuffer")
	void PutImage(const FString& file, int x, int y, int width, int height);
	UFUNCTION(BlueprintCallable, Category = "CanvasBuffer")
	void PutString(const FString& text, int x, int y);

	// 由UE输入系统调用，推送键盘事件（Blueprint侧从KeyEvent用Get Key取FKey传入）
	UFUNCTION(BlueprintCallable, Category = "CanvasBuffer")
	void ReceiveKeyDown(FKey key);
	UFUNCTION(BlueprintCallable, Category = "CanvasBuffer")
	void ReceiveKeyUp(FKey key);

	// 将FKey转为BiosKey可比较的整数（Blueprint侧用于与BiosKey返回值比较）
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CanvasBuffer")
	static int32 FKeyCode(FKey key);

	// 由小游戏逻辑调用，读取键盘事件
	UFUNCTION(BlueprintCallable, Category = "CanvasBuffer")
	int32 BiosKey(int32 cmd);
	UFUNCTION(BlueprintCallable, Category = "CanvasBuffer")
	void ClearKeyBuffer();

	// 由UE输入系统调用，推送鼠标事件（Blueprint侧从PointerEvent取坐标和按键）
	UFUNCTION(BlueprintCallable, Category = "CanvasBuffer")
	void ReceiveMouseMove(int32 x, int32 y);
	UFUNCTION(BlueprintCallable, Category = "CanvasBuffer")
	void ReceiveMouseButtonDown(int32 button);
	UFUNCTION(BlueprintCallable, Category = "CanvasBuffer")
	void ReceiveMouseButtonUp(int32 button);

	// 将FKey鼠标按键转为按键索引（左=0 右=1 中=2，未知=-1）
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CanvasBuffer")
	static int32 FMouseButton(FKey key);

	// 由小游戏逻辑调用，读取鼠标事件和状态
	UFUNCTION(BlueprintCallable, Category = "CanvasBuffer")
	int32 BiosMouse(int32 cmd);
	UFUNCTION(BlueprintCallable, Category = "CanvasBuffer")
	void ClearMouseBuffer();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CanvasBuffer")
	int32 GetMouseX() const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CanvasBuffer")
	int32 GetMouseY() const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CanvasBuffer")
	bool IsMouseButtonDown(int32 button) const;

	Canvas* GetCanvas();

private:
	Canvas canvas;

	UPROPERTY()
	UTexture2D* tex = nullptr;
};

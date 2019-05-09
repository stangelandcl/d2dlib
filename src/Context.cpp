#include <stack>
using namespace std;


typedef struct D2DContext
{
	ID2D1Factory* factory;
	IDWriteFactory* writeFactory;
	IWICImagingFactory* imageFactory;

	union
	{
		struct // HwndRenderTarget
		{
			ID2D1HwndRenderTarget* renderTarget;
			HWND hwnd;
		};

		struct // BitmapRenderTarget
		{
			ID2D1BitmapRenderTarget *bitmapRenderTarget;
			ID2D1Bitmap* bitmap;
		};
	};

	std::stack<D2D1_MATRIX_3X2_F>* matrixStack;

	HRESULT lastErrorCode;

} D2DContext;


template<class Interface>
inline void SafeRelease(Interface **ppInterfaceToRelease)
{
  if (*ppInterfaceToRelease != NULL)
  {
    (*ppInterfaceToRelease)->Release();
    (*ppInterfaceToRelease) = NULL;
  }
}

#define RetrieveContext(ctx) D2DContext* context = reinterpret_cast<D2DContext*>(ctx)
#define RetrieveD2DBitmap(ctx) ID2D1Bitmap* d2dbitmap = reinterpret_cast<ID2D1Bitmap*>(d2dbitmapHandle)


static void ReleaseObject(void* handle)
{
    ID2D1Resource* object = reinterpret_cast<ID2D1Resource*>(handle);
    SafeRelease(&object);
}

static void* CreateContext(HWND hwnd)
{
	D2DContext* context = new D2DContext();
	ZeroMemory(context, sizeof(context));
	HRESULT hr;

	context->hwnd = hwnd;

	//context->solidBrushes = new map<UINT32, ID2D1SolidColorBrush*>();
	context->matrixStack = new std::stack<D2D1_MATRIX_3X2_F>();

	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &context->factory);

	if (!SUCCEEDED(hr)) {
		context->lastErrorCode = hr;
		return NULL;
	}

	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(context->writeFactory), reinterpret_cast<IUnknown **>(&context->writeFactory));

	if (!SUCCEEDED(hr)) {
		context->lastErrorCode = hr;
		return NULL;
	}

	hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER,
			IID_IWICImagingFactory, (LPVOID*)&context->imageFactory);

	if (!SUCCEEDED(hr)) {
		context->lastErrorCode = hr;
		return NULL;
	}

	//D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);
	D2D1_SIZE_U size = D2D1::SizeU(1024, 768);

	hr = context->factory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(context->hwnd, size),
			&context->renderTarget);

	if (!SUCCEEDED(hr)) {
		context->lastErrorCode = hr;
		return NULL;
	}

	//context->renderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE::D2D1_ANTIALIAS_MODE_ALIASED);
	context->renderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE::D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

	return (void*)context;
}


// This is the constructor of a class that has been exported.
// see D2DLib.h for the class definition
//CD2DLib::CD2DLib()
//{
//	return;
//}

// This is an example of an exported function.

static void DestroyContext(void* handle)
{
	D2DContext* context = reinterpret_cast<D2DContext*>(handle);

	delete context->matrixStack;

	SafeRelease(&context->imageFactory);
	SafeRelease(&context->writeFactory);
	SafeRelease(&context->renderTarget);
	SafeRelease(&context->factory);

	//if (context->solidBrushes != NULL)
	//{
	//	for( std::map<UINT32, ID2D1SolidColorBrush*>::iterator it = context->solidBrushes->begin();
	//		it != context->solidBrushes->end(); it++)
	//	{
	//		SafeRelease(&context->solidBrushes->at(it->first));
	//	}

	//	delete context->solidBrushes;
	//}

	delete context;
}

static void ResizeContext(void* handle)
{
	if (handle == 0 || handle == INVALID_HANDLE_VALUE) return;

	D2DContext* context = reinterpret_cast<D2DContext*>(handle);

	if (context->renderTarget == NULL || context->hwnd == INVALID_HANDLE_VALUE) return;

	RECT rc;
	GetClientRect(context->hwnd, &rc);

	D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

	context->renderTarget->Resize(size);
}

static void SetContextProperties(void* ctx, D2D1_ANTIALIAS_MODE antialiasMode)
{
	RetrieveContext(ctx);

	context->renderTarget->SetAntialiasMode(antialiasMode);
}

static void BeginRender(void* ctx)
{
	RetrieveContext(ctx);

	context->renderTarget->BeginDraw();
}

static void BeginRenderWithBackgroundColor(void* ctx, D2D1_COLOR_F backColor)
{
	RetrieveContext(ctx);

	context->renderTarget->BeginDraw();

	context->renderTarget->Clear(backColor);
}

static void BeginRenderWithBackgroundBitmap(void* ctx, void* bitmap)
{
	RetrieveContext(ctx);
	ID2D1Bitmap* d2dbitmap = reinterpret_cast<ID2D1Bitmap*>(bitmap);

	context->renderTarget->BeginDraw();

	D2D1_SIZE_F size = context->renderTarget->GetSize();
	D2D1_RECT_F destRect = {0, 0, size.width, size.height};

	context->renderTarget->DrawBitmap(d2dbitmap, &destRect);
}

static void Clear(void* ctx, D2D1_COLOR_F color)
{
	RetrieveContext(ctx);

	context->renderTarget->Clear(color);
}

static void EndRender(void* ctx)
{
	RetrieveContext(ctx);
	context->renderTarget->EndDraw();
}

static void Flush(void* ctx)
{
	RetrieveContext(ctx);
	context->renderTarget->Flush();
}
//D2DLIB_API void* DeleteStorkeStyle(const void* handle)
//{
//
//	D2DContext* context = reinterpret_cast<D2DContext*>(handle);
//
//	ID2D1StrokeStyle* strokeStyle;
//
//	context->factory->CreateStrokeStyle(D2D1::StrokeStyleProperties(
//            D2D1_CAP_STYLE_FLAT,
//            D2D1_CAP_STYLE_FLAT,
//            D2D1_CAP_STYLE_ROUND,
//            D2D1_LINE_JOIN_MITER,
//            10.0f,
//            D2D1_DASH_STYLE_CUSTOM,
//            0.0f), dashes,
//        ARRAYSIZE(dashes), &strokeStyle);
//
//	return (void*)strokeStyle;
//}


static void* CreateBitmapRenderTarget(void* ctx, D2D1_SIZE_F size)
{
	RetrieveContext(ctx);

	D2DContext* bitmapRenderTargetContext = new D2DContext();
	ZeroMemory(bitmapRenderTargetContext, sizeof(D2DContext));

	bitmapRenderTargetContext->factory = context->factory;
	bitmapRenderTargetContext->imageFactory = context->imageFactory;
	bitmapRenderTargetContext->writeFactory = context->writeFactory;

	HRESULT hr;

	if (size.width <= 0 && size.height <= 0)
	{
		hr = context->renderTarget->CreateCompatibleRenderTarget(
			&bitmapRenderTargetContext->bitmapRenderTarget);
	}
	else
	{
		hr = context->renderTarget->CreateCompatibleRenderTarget(
			size, &bitmapRenderTargetContext->bitmapRenderTarget);
	}

	if (!SUCCEEDED(hr)) {
		context->lastErrorCode = hr;
		return NULL;
	}

	return (void*)bitmapRenderTargetContext;
}

static void DrawBitmapRenderTarget(void* ctx, void* bitmapRenderTargetHandle, D2D1_RECT_F* rect,
														float opacity, D2D1_BITMAP_INTERPOLATION_MODE interpolationMode)
{
	RetrieveContext(ctx);
	D2DContext* bitmapRenderTargetContext = reinterpret_cast<D2DContext*>(bitmapRenderTargetHandle);

	if (bitmapRenderTargetContext->bitmap == NULL)
	{
		bitmapRenderTargetContext->bitmapRenderTarget->GetBitmap(&bitmapRenderTargetContext->bitmap);
	}

	context->renderTarget->DrawBitmap(bitmapRenderTargetContext->bitmap, rect, opacity, interpolationMode);
}

static void* GetBitmapRenderTargetBitmap(void* bitmapRenderTargetHandle)
{
	RetrieveContext(bitmapRenderTargetHandle);

	ID2D1Bitmap* bitmap = NULL;
	context->bitmapRenderTarget->GetBitmap(&bitmap);

	return bitmap;
}

static void DestoryBitmapRenderTarget(void* ctx)
{
	if (ctx == 0) return;

	RetrieveContext(ctx);

	if (context->bitmap != NULL)
	{
		SafeRelease(&context->bitmap);
	}

	if (context->bitmapRenderTarget != NULL)
	{
		SafeRelease(&context->bitmapRenderTarget);
	}

	delete context;
}

static void PushClip(void* ctx, D2D1_RECT_F* rect, D2D1_ANTIALIAS_MODE antiAliasMode)
{
	D2DContext* context = reinterpret_cast<D2DContext*>(ctx);
	context->renderTarget->PushAxisAlignedClip(rect, antiAliasMode);
}

static void PopClip(void* ctx)
{
	D2DContext* context = reinterpret_cast<D2DContext*>(ctx);
	context->renderTarget->PopAxisAlignedClip();
}

static void PushTransform(void* ctx)
{
	RetrieveContext(ctx);

	D2D1_MATRIX_3X2_F matrix;

	context->renderTarget->GetTransform(&matrix);
	context->matrixStack->push(matrix);
}

static void PopTransform(void* ctx)
{
	RetrieveContext(ctx);

	D2D1_MATRIX_3X2_F matrix = context->matrixStack->top();
	context->matrixStack->pop();

	context->renderTarget->SetTransform(&matrix);
}

static void TranslateTransform(void* ctx, float x, float y)
{
	RetrieveContext(ctx);

	D2D1_MATRIX_3X2_F matrix;
	context->renderTarget->GetTransform(&matrix);

	D2D1_MATRIX_3X2_F translateMatrix = D2D1::Matrix3x2F::Translation(x, y);
	context->renderTarget->SetTransform(matrix * translateMatrix);
}

static void ScaleTransform(void* ctx, float scaleX, float scaleY, D2D1_POINT_2F center)
{
	RetrieveContext(ctx);

	D2D1_MATRIX_3X2_F matrix;
	context->renderTarget->GetTransform(&matrix);

	D2D1_MATRIX_3X2_F scaleMatrix = D2D1::Matrix3x2F::Scale(scaleX, scaleY, center);

	context->renderTarget->SetTransform(scaleMatrix * matrix);
}

static void RotateTransform(void* ctx, float angle, D2D_POINT_2F point)
{
	RetrieveContext(ctx);

	D2D1_MATRIX_3X2_F matrix;
	context->renderTarget->GetTransform(&matrix);

	D2D1_MATRIX_3X2_F rotateMatrix = D2D1::Matrix3x2F::Rotation(angle, point);
	context->renderTarget->SetTransform(rotateMatrix * matrix);
}

static void SkewTransform(void* ctx, float angleX, float angleY, D2D1_POINT_2F center)
{
	RetrieveContext(ctx);

	D2D1_MATRIX_3X2_F matrix = D2D1::Matrix3x2F::Skew(angleX, angleY, center);
	context->renderTarget->SetTransform(matrix);
}

static void SetTransform(void* ctx, float angle, D2D_POINT_2F center)
{
	RetrieveContext(ctx);

	D2D1::Matrix3x2F mr = D2D1::Matrix3x2F::Rotation(angle, D2D1::Point2F(0, 0));
	D2D1::Matrix3x2F mt = D2D1::Matrix3x2F::Translation(center.x, center.y);

	context->renderTarget->SetTransform(mr * mt);
}

static void ResetTransform(void* ctx)
{
	RetrieveContext(ctx);
	context->renderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
}



static HRESULT GetLastErrorCode(void* handle)
{
	D2DContext* context = reinterpret_cast<D2DContext*>(handle);
	return context->lastErrorCode;
}

static void* CreateLayer(void* ctx)
{
	RetrieveContext(ctx);

	ID2D1Layer* layer;
	context->renderTarget->CreateLayer(&layer);

	return (void*)layer;
}

static void PushLayer(void* ctx, void* layerHandle, D2D1_RECT_F& contentBounds, ID2D1Brush* opacityBrush,
							 D2D1_LAYER_OPTIONS layerOptions)
{
	RetrieveContext(ctx);
	ID2D1Layer* layer = reinterpret_cast<ID2D1Layer*>(layerHandle);
	context->renderTarget->PushLayer(
		D2D1::LayerParameters(contentBounds, NULL, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
		D2D1::IdentityMatrix(), 1, opacityBrush, layerOptions), layer);
}

static void* CreateBitmapFromFile(void* ctx,
  PCWSTR uri, uint32_t destinationWidth, uint32_t destinationHeight, ID2D1Bitmap **ppBitmap)
{
	D2DContext* context = reinterpret_cast<D2DContext*>(ctx);

	IWICBitmapDecoder *pDecoder = NULL;
	IWICBitmapFrameDecode *pSource = NULL;
	IWICStream *pStream = NULL;
	IWICFormatConverter *pConverter = NULL;
	//IWICBitmapScaler *pScaler = NULL;

	HRESULT hr = context->imageFactory->CreateDecoderFromFilename(
		uri, NULL, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &pDecoder);

	if (SUCCEEDED(hr))
  {
    hr = pDecoder->GetFrame(0, &pSource);
  }

	if (SUCCEEDED(hr))
  {
    // Convert the image format to 32bppPBGRA
    // (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
    hr = context->imageFactory->CreateFormatConverter(&pConverter);
	}

	if (SUCCEEDED(hr))
  {
		hr = pConverter->Initialize(pSource, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone,
			NULL, 0.f, WICBitmapPaletteTypeMedianCut);
	}

	if (SUCCEEDED(hr))
  {
		hr = context->renderTarget->CreateBitmapFromWicBitmap(pConverter, NULL, ppBitmap);
	}

	SafeRelease(&pDecoder);
  SafeRelease(&pSource);
  SafeRelease(&pStream);
  SafeRelease(&pConverter);
  //SafeRelease(&pScaler);

	return (void*)0;
}

static void* CreateBitmapFromHBitmap(void* ctx, HBITMAP hBitmap, BOOL alpha)
{
	RetrieveContext(ctx);

	IWICBitmap* wicBitmap = NULL;
	HRESULT hr = context->imageFactory->CreateBitmapFromHBITMAP(hBitmap, NULL,
		alpha ? WICBitmapAlphaChannelOption::WICBitmapUsePremultipliedAlpha
		: WICBitmapAlphaChannelOption::WICBitmapIgnoreAlpha, &wicBitmap);

	if (!SUCCEEDED(hr)) {
		context->lastErrorCode = hr;
		return NULL;
	}

	IWICFormatConverter *wicConverter = NULL;
	ID2D1Bitmap* d2dBitmap = NULL;
	IWICBitmapSource* wicBitmapSource = NULL;

	if (alpha) {
		hr = context->imageFactory->CreateFormatConverter(&wicConverter);

		if (!SUCCEEDED(hr)) {
			context->lastErrorCode = hr;
			return NULL;
		}

		hr = wicConverter->Initialize(wicBitmap, GUID_WICPixelFormat32bppPRGBA, WICBitmapDitherTypeNone,
			NULL, 0.f, WICBitmapPaletteTypeMedianCut);

		if (!SUCCEEDED(hr)) {
			SafeRelease(&wicBitmap);
			context->lastErrorCode = hr;
			return NULL;
		}

		wicBitmapSource = wicConverter;
	}
	else {
		wicBitmapSource = wicBitmap;
	}

	hr = context->renderTarget->CreateBitmapFromWicBitmap(wicBitmapSource, NULL, &d2dBitmap);

	SafeRelease(&wicBitmap);
	SafeRelease(&wicConverter);

	if (!SUCCEEDED(hr)) {
		context->lastErrorCode = hr;
		return NULL;
	}

	return (void*)d2dBitmap;
}

static void* CreateBitmapFromMemory(void* ctx, uint32_t width, uint32_t height, uint32_t stride, uint8_t* buffer, uint32_t offset, uint32_t length)
{
	RetrieveContext(ctx);

	IWICBitmap* wicBitmap = NULL;

	HRESULT hr = context->imageFactory->CreateBitmapFromMemory(width, height,
		GUID_WICPixelFormat32bppPBGRA, stride, length, buffer, &wicBitmap);

	ID2D1Bitmap* d2dBitmap = NULL;
	hr = context->renderTarget->CreateBitmapFromWicBitmap(wicBitmap, NULL, &d2dBitmap);

	SafeRelease(&wicBitmap);

	return (void*)d2dBitmap;
}

static void* CreateBitmapFromBytes(void* ctx, uint8_t* buffer, uint32_t offset, uint32_t length)
{
	RetrieveContext(ctx);

	IWICImagingFactory* imageFactory = context->imageFactory;

	IWICBitmapDecoder *decoder = NULL;
	IWICBitmapFrameDecode *source = NULL;
	IWICStream *stream = NULL;
	IWICFormatConverter *converter = NULL;
	IWICBitmapScaler *scaler = NULL;

	ID2D1Bitmap* bitmap = NULL;

	HRESULT hr = S_OK;

	if (SUCCEEDED(hr))
	{
		// Create a WIC stream to map onto the memory.
		hr = imageFactory->CreateStream(&stream);
	}

	if (SUCCEEDED(hr))
	{
		// Initialize the stream with the memory pointer and size.
		hr = stream->InitializeFromMemory(reinterpret_cast<uint8_t*>(buffer + offset), length);
	}

	if (SUCCEEDED(hr))
	{
		// Create a decoder for the stream.
		hr = imageFactory->CreateDecoderFromStream(stream, NULL, WICDecodeMetadataCacheOnLoad, &decoder);
	}

	if (SUCCEEDED(hr))
	{
		// Create the initial frame.
		hr = decoder->GetFrame(0, &source);
	}

	if (SUCCEEDED(hr))
	{
		// Convert the image format to 32bppPBGRA
		// (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
		hr = imageFactory->CreateFormatConverter(&converter);
	}

	if (SUCCEEDED(hr))
  {
		hr = converter->Initialize(source, GUID_WICPixelFormat32bppPBGRA,
			WICBitmapDitherTypeNone, NULL, 0.f, WICBitmapPaletteTypeMedianCut);
	}

	if (SUCCEEDED(hr))
	{
		//create a Direct2D bitmap from the WIC bitmap.
		hr = context->renderTarget->CreateBitmapFromWicBitmap(converter, NULL, &bitmap);
	}

	SafeRelease(&decoder);
	SafeRelease(&source);
	SafeRelease(&stream);
	SafeRelease(&converter);
	SafeRelease(&scaler);

	return (void*)bitmap;
}

static void* CreateBitmapFromFile(void* ctx, LPCWSTR filepath)
{
	RetrieveContext(ctx);

	IWICImagingFactory* imageFactory = context->imageFactory;

	IWICBitmapDecoder *decoder = NULL;
	IWICBitmapFrameDecode *source = NULL;
	IWICStream *stream = NULL;
	IWICFormatConverter *converter = NULL;
	IWICBitmapScaler *scaler = NULL;

	ID2D1Bitmap* bitmap = NULL;

	HRESULT hr = S_OK;

	if (SUCCEEDED(hr))
	{
		hr = imageFactory->CreateStream(&stream);
	}

	if (SUCCEEDED(hr))
	{
		hr = stream->InitializeFromFilename(filepath, GENERIC_READ);
	}

	if (SUCCEEDED(hr))
	{
		hr = imageFactory->CreateDecoderFromStream(stream, NULL, WICDecodeMetadataCacheOnLoad, &decoder);
	}

	if (SUCCEEDED(hr))
	{
		hr = decoder->GetFrame(0, &source);
	}

	if (SUCCEEDED(hr))
	{
		hr = imageFactory->CreateFormatConverter(&converter);
	}

	if (SUCCEEDED(hr))
  {
		hr = converter->Initialize(source, GUID_WICPixelFormat32bppPBGRA,
			WICBitmapDitherTypeNone, NULL, 0.f, WICBitmapPaletteTypeMedianCut);
	}

	if (SUCCEEDED(hr))
	{
		hr = context->renderTarget->CreateBitmapFromWicBitmap(converter, NULL, &bitmap);
	}

	SafeRelease(&decoder);
	SafeRelease(&source);
	SafeRelease(&stream);
	SafeRelease(&converter);
	SafeRelease(&scaler);

	return (void*)bitmap;
}

static void DrawGDIBitmapRect(void* hContext, HBITMAP hBitmap, D2D1_RECT_F* rect,
    D2D1_RECT_F* sourceRectangle, float opacity, BOOL alpha,
    D2D1_BITMAP_INTERPOLATION_MODE interpolationMode)
{
	D2DContext* context = reinterpret_cast<D2DContext*>(hContext);

	IWICBitmap* bitmap = NULL;

	context->imageFactory->CreateBitmapFromHBITMAP(hBitmap, 0,
		alpha ? WICBitmapAlphaChannelOption::WICBitmapUseAlpha
		: WICBitmapAlphaChannelOption::WICBitmapIgnoreAlpha, &bitmap);

	ID2D1Bitmap* d2dBitmap = NULL;

	HRESULT hr = context->renderTarget->CreateBitmapFromWicBitmap(bitmap, NULL, &d2dBitmap);

	_ASSERT(d2dBitmap != NULL);

	context->renderTarget->DrawBitmap(d2dBitmap, rect, opacity, interpolationMode, sourceRectangle);

	SafeRelease(&d2dBitmap);
	SafeRelease(&bitmap);
}

static void DrawGDIBitmap(void* hContext, HBITMAP hBitmap, float opacity, BOOL alpha,
    D2D1_BITMAP_INTERPOLATION_MODE interpolationMode)
{
	DrawGDIBitmapRect(hContext, hBitmap, NULL, NULL, opacity, alpha, interpolationMode);
}

static void DrawD2DBitmap(void* ctx, void* d2dbitmap, D2D1_RECT_F* destRect, D2D1_RECT_F* srcRect,
	float opacity, D2D1_BITMAP_INTERPOLATION_MODE interpolationMode)
{
	RetrieveContext(ctx);
	ID2D1Bitmap* bitmap = reinterpret_cast<ID2D1Bitmap*>(d2dbitmap);

	context->renderTarget->DrawBitmap(bitmap, destRect, opacity, interpolationMode, srcRect);
}

static D2D1_SIZE_F GetBitmapSize(void* d2dbitmap)
{
	ID2D1Bitmap* bitmap = reinterpret_cast<ID2D1Bitmap*>(d2dbitmap);
	return bitmap->GetSize();
}

#ifdef __unused__
void _unused_get_wic_info() {

	TCHAR msg[256];

	uint32_t w, h;
	wicBitmap->GetSize(&w, &h);
	_swprintf_c(msg, 256, L"width: %d, height: %d", w, h);
	MessageBox(context->hwnd, msg, NULL, MB_OK);

	WICPixelFormatGUID pf;
	wicBitmap->GetPixelFormat(&pf);
	_swprintf_c(msg, 256, L"%X-%X-%X-%X-%X-%X-%X-%X-%X-%X-%X", pf.Data1, pf.Data2, pf.Data3,
		pf.Data4[0], pf.Data4[1], pf.Data4[2], pf.Data4[3], pf.Data4[4], pf.Data4[5], pf.Data4[6], pf.Data4[7]);
	MessageBox(context->hwnd, msg, NULL, MB_OK);
}
#endif /* __unused */

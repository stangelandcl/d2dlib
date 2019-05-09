typedef struct D2DPen
{
	ID2D1SolidColorBrush* brush;
	ID2D1StrokeStyle* strokeStyle;
} D2DPen;

static  void* CreatePenStroke(void* ctx, D2D1_COLOR_F color, D2D1_DASH_STYLE dashStyle)
{
	RetrieveContext(ctx);

	ID2D1SolidColorBrush* brush;
	(context->renderTarget)->CreateSolidColorBrush(color, &brush);

	ID2D1StrokeStyle* strokeStyle = NULL;

	if (dashStyle != D2D1_DASH_STYLE_SOLID)
	{
		context->factory->CreateStrokeStyle(D2D1::StrokeStyleProperties(
			D2D1_CAP_STYLE_FLAT,
			D2D1_CAP_STYLE_FLAT,
			D2D1_CAP_STYLE_ROUND,
			D2D1_LINE_JOIN_MITER,
			10.0f,
			dashStyle,
			0.0f), NULL, 0, &strokeStyle);
	}

	D2DPen* pen = new D2DPen();
	pen->brush = brush;
	pen->strokeStyle = strokeStyle;

	return (void*)pen;
}

static void DestoryPenStroke(void* penHandle)
{
	D2DPen* pen = (D2DPen*)penHandle;

	SafeRelease(&pen->strokeStyle);
	SafeRelease(&pen->brush);
}

static void* CreateStorkeStyle(void* ctx, float* dashes, uint32_t dashCount)
{
	RetrieveContext(ctx);

	ID2D1StrokeStyle* strokeStyle;

	context->factory->CreateStrokeStyle(D2D1::StrokeStyleProperties(
            D2D1_CAP_STYLE_FLAT,
            D2D1_CAP_STYLE_FLAT,
            D2D1_CAP_STYLE_ROUND,
            D2D1_LINE_JOIN_MITER,
            10.0f,
						D2D1_DASH_STYLE_CUSTOM,
            0.0f), dashes, dashCount, &strokeStyle);

	return (void*)strokeStyle;
}

static void* CreateSolidColorBrush(void* ctx, D2D1_COLOR_F color)
{
	RetrieveContext(ctx);

	ID2D1SolidColorBrush* brush;
	context->renderTarget->CreateSolidColorBrush(color, &brush);

	return (void*)brush;
}

static void SetSolidColorBrushColor(void* brushHandle, D2D1_COLOR_F* color)
{
	ID2D1SolidColorBrush* brush = reinterpret_cast<ID2D1SolidColorBrush*>(brushHandle);
	brush->SetColor(color);
}

static void* CreateRadialGradientBrush(void* ctx, D2D1_POINT_2F origin,
    D2D1_POINT_2F offset, float radiusX, float radiusY,
    D2D1_GRADIENT_STOP* gradientStops, uint32_t gradientStopCount)
{
	RetrieveContext(ctx);
	ID2D1RenderTarget* renderTarget = context->renderTarget;
	HRESULT hr;

	ID2D1GradientStopCollection *gradientStopCollection = NULL;

  hr = renderTarget->CreateGradientStopCollection(
		gradientStops, gradientStopCount, &gradientStopCollection);

	ID2D1RadialGradientBrush* radialGradientBrush = NULL;

	if (SUCCEEDED(hr))
	{
		hr = renderTarget->CreateRadialGradientBrush(D2D1::RadialGradientBrushProperties(
			origin, offset, radiusX, radiusY), gradientStopCollection, &radialGradientBrush);
	}

	return (void*)radialGradientBrush;
}

static  void DrawString(void* ctx, LPCWSTR text, D2D1_COLOR_F color,
													 LPCWSTR fontName, float fontSize, D2D1_RECT_F* rect,
													 DWRITE_TEXT_ALIGNMENT halign, DWRITE_PARAGRAPH_ALIGNMENT valign)
{
	RetrieveContext(ctx);

	IDWriteTextFormat* textFormat = NULL;

	// Create a DirectWrite text format object.
  context->writeFactory->CreateTextFormat(fontName,
			NULL,
      DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
      fontSize,
      L"", //locale
      &textFormat);

	textFormat->SetTextAlignment(halign);
	textFormat->SetParagraphAlignment(valign);

	// Create a solid color brush.
	ID2D1SolidColorBrush* brush;
	(context->renderTarget)->CreateSolidColorBrush(color, &brush);

	context->renderTarget->DrawText(text, wcslen(text), textFormat, rect, brush);

	SafeRelease(&brush);
	SafeRelease(&textFormat);
}

static void DrawGlyphRun(void* ctx, D2D1_POINT_2F baselineOrigin,
			const DWRITE_GLYPH_RUN *glyphRun, D2D1_COLOR_F color,
			DWRITE_MEASURING_MODE measuringMode)
{
	D2DContext* context = reinterpret_cast<D2DContext*>(ctx);

}


static void CreateEffect(void* ctx)
{
	RetrieveContext(ctx);

//	ID2D1Effect perspectiveTransformEffect;
//	m_d2dContext->CreateEffect(CLSID_D2D13DPerspectiveTransform, &perspectiveTransformEffect);
//
//perspectiveTransformEffect->SetInput(0, bitmap);
//
//perspectiveTransformEffect->SetValue(D2D1_3DPERSPECTIVETRANSFORM_PROP_PERSPECTIVE_ORIGIN, D2D1::Vector3F(0.0f, 192.0f, 0.0f));
//perspectiveTransformEffect->SetValue(D2D1_3DPERSPECTIVETRANSFORM_PROP_ROTATION, D2D1::Vector3F(0.0f, 30.0f, 0.0f));
//
//m_d2dContext->BeginDraw();
//m_d2dContext->DrawImage(perspectiveTransformEffect.Get());
//m_d2dContext->EndDraw();

}

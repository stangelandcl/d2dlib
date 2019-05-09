static void DrawLine(void* ctx, D2D1_POINT_2F start, D2D1_POINT_2F end, D2D1_COLOR_F color,
	float width, D2D1_DASH_STYLE dashStyle)
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

	context->renderTarget->DrawLine(start, end, brush, width, strokeStyle);

	SafeRelease(&strokeStyle);
	SafeRelease(&brush);
}

static void DrawArrowLine(void* ctx, D2D1_POINT_2F start, D2D1_POINT_2F end, D2D1_COLOR_F color,
	float width, D2D1_DASH_STYLE dashStyle)
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

	context->renderTarget->DrawLine(start, end, brush, width, strokeStyle);

	SafeRelease(&strokeStyle);
	SafeRelease(&brush);
}

static void DrawLineWithPen(void* ctx, D2D1_POINT_2F start, D2D1_POINT_2F end, void* penHandle, float width)
{
	RetrieveContext(ctx);

	D2DPen* pen = (D2DPen*)penHandle;

	context->renderTarget->DrawLine(start, end, pen->brush, width, pen->strokeStyle);
}

static void DrawLines(void* ctx, D2D1_POINT_2F* points, uint32_t count, D2D1_COLOR_F color,
	float width, D2D1_DASH_STYLE dashStyle)
{
	if (count <= 1) return;

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

		ID2D1PathGeometry* pathGeo = NULL;
		context->factory->CreatePathGeometry(&pathGeo);

		ID2D1GeometrySink* sink = NULL;
		pathGeo->Open(&sink);
		sink->BeginFigure(points[0], D2D1_FIGURE_BEGIN::D2D1_FIGURE_BEGIN_FILLED);
		sink->AddLines(points + 1, count - 1);
		//sink->EndFigure(D2D1_FIGURE_END_CLOSED);
		sink->Close();

		context->renderTarget->DrawGeometry(pathGeo, brush, width, strokeStyle);

		SafeRelease(&sink);
		SafeRelease(&pathGeo);
	}
	else
	{
		for(uint32_t i = 0;i < count-1; i++)
		{
			context->renderTarget->DrawLine(points[i], points[i + 1], brush, width, strokeStyle);
		}
		//context->renderTarget->DrawLine(points[count - 1], points[0], brush, weight, strokeStyle);
	}

	SafeRelease(&strokeStyle);
	SafeRelease(&brush);
}

static void DrawRectangle(void* handle, D2D1_RECT_F* rect, D2D1_COLOR_F color,
	float width, D2D1_DASH_STYLE dashStyle)
{
	D2DContext* context = reinterpret_cast<D2DContext*>(handle);

	ID2D1SolidColorBrush* brush;

	// Create a black brush.
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

	context->renderTarget->DrawRectangle(rect, brush, width, strokeStyle);

	SafeRelease(&strokeStyle);
	SafeRelease(&brush);
}

static void FillRectangle(void* ctx, D2D1_RECT_F* rect, D2D1_COLOR_F color)
{
	RetrieveContext(ctx);

	//UINT32 colorValue = static_cast<UINT32>(color->a*255) << 24
	//	| static_cast<UINT32>(color->r*255) << 16
	//	| static_cast<UINT32>(color->g*255) << 8
	//	| static_cast<UINT32>(color->b*255);

	//ID2D1SolidColorBrush* brush;

	//std::map<UINT32, ID2D1SolidColorBrush*>::iterator it = context->solidBrushes->find(colorValue);
	//if (it == context->solidBrushes->end())
	//{
	//	lastResultCode = (context->renderTarget)->CreateSolidColorBrush(*color, &brush);
	//	context->solidBrushes->insert(context->solidBrushes->begin(),
	//		std::pair<UINT32, ID2D1SolidColorBrush*>(colorValue, brush));
	//}
	//else
	//{
	//	brush = it->second;
	//}

	// Create a black brush.
	ID2D1SolidColorBrush* brush;
	(context->renderTarget)->CreateSolidColorBrush(color, &brush);

	context->renderTarget->FillRectangle(rect, brush);

	SafeRelease(&brush);
}

static void DrawEllipse(void* handle, D2D1_ELLIPSE* ellipse, D2D1_COLOR_F color,
    float width, D2D1_DASH_STYLE dashStyle)
{
	D2DContext* context = reinterpret_cast<D2DContext*>(handle);

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

	context->renderTarget->DrawEllipse(ellipse, brush, width, strokeStyle);

	SafeRelease(&strokeStyle);
	SafeRelease(&brush);
}

static void FillEllipse(void* handle, D2D1_ELLIPSE* ellipse, D2D1_COLOR_F color)
{
	D2DContext* context = reinterpret_cast<D2DContext*>(handle);

	// Create a black brush.
	ID2D1SolidColorBrush* brush;
	context->renderTarget->CreateSolidColorBrush(color, &brush);

	context->renderTarget->FillEllipse(ellipse, brush);

	SafeRelease(&brush);
}

static void FillEllipseWithBrush(void* ctx, D2D1_ELLIPSE* ellipse, void* brush_handle)
{
	RetrieveContext(ctx);

	ID2D1Brush* brush = reinterpret_cast<ID2D1Brush*>(brush_handle);
	context->renderTarget->FillEllipse(ellipse, brush);
}

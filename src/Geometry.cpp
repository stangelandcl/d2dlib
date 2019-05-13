typedef struct D2DPathContext
{
	D2DContext* d2context;
	ID2D1PathGeometry* path;
	ID2D1GeometrySink* sink;
	bool isOpen;
	bool isClosed;
} D2DPathContext;

static void* CreateRectangleGeometry(void* ctx, D2D1_RECT_F& rect)
{
	RetrieveContext(ctx);

	ID2D1RectangleGeometry* rectGeo;
	context->factory->CreateRectangleGeometry(rect, &rectGeo);

	return (void*)rectGeo;
}

static void* CreatePathGeometry(void* ctx)
{
	D2DContext* context = reinterpret_cast<D2DContext*>(ctx);

	D2DPathContext* pathContext = new D2DPathContext();
	context->factory->CreatePathGeometry(&pathContext->path);
	pathContext->path->Open(&pathContext->sink);

	pathContext->sink->SetFillMode(D2D1_FILL_MODE_WINDING);

	pathContext->d2context = context;

	return (void*)pathContext;
}

static void DestroyPathGeometry(void* ctx)
{
	D2DPathContext* pathContext = reinterpret_cast<D2DPathContext*>(ctx);
	SafeRelease(&pathContext->path);

	delete pathContext;
}

static void ClosePath(void* ctx)
{
	D2DPathContext* pathContext = reinterpret_cast<D2DPathContext*>(ctx);
	pathContext->sink->EndFigure(D2D1_FIGURE_END_CLOSED);
	pathContext->sink->Close();
	pathContext->isClosed = true;

	SafeRelease(&pathContext->sink);
}

static void AddPathLines(void* ctx, D2D1_POINT_2F* points, uint32_t count)
{
	D2DPathContext* pathContext = reinterpret_cast<D2DPathContext*>(ctx);

	if (!pathContext->isOpen)
	{
		pathContext->sink->BeginFigure(points[0], D2D1_FIGURE_BEGIN_FILLED);
		pathContext->isOpen = true;
	}

	pathContext->sink->AddLines(points, count);
}

static void AddPathBeziers(void* ctx, D2D1_BEZIER_SEGMENT* bezierSegments, uint32_t count)
{
	D2DPathContext* pathContext = reinterpret_cast<D2DPathContext*>(ctx);

	if (!pathContext->isOpen)
	{
		pathContext->sink->BeginFigure(bezierSegments[0].point1, D2D1_FIGURE_BEGIN_FILLED);
		pathContext->isOpen = true;
	}

	pathContext->sink->AddBeziers(bezierSegments, count);
}

static void AddPathEllipse(void* ctx, const D2D1_ELLIPSE* ellipse)
{
	D2DPathContext* pathContext = reinterpret_cast<D2DPathContext*>(ctx);

	if (!pathContext->isOpen)
	{
		D2D1_POINT_2F p;
		p.x = ellipse->point.x;
		p.y = ellipse->point.y - ellipse->radiusY / 2;

		pathContext->sink->BeginFigure(p, D2D1_FIGURE_BEGIN_HOLLOW);
		pathContext->isOpen = true;
	}

	D2D1_ARC_SEGMENT seg;
	seg.rotationAngle = 0.0;
	seg.arcSize = D2D1_ARC_SIZE::D2D1_ARC_SIZE_LARGE;
	seg.point = ellipse->point;
	seg.size = D2D1::SizeF(ellipse->radiusX, ellipse->radiusY);
	seg.sweepDirection = D2D1_SWEEP_DIRECTION::D2D1_SWEEP_DIRECTION_CLOCKWISE;
	pathContext->sink->AddArc(&seg);
}

static void AddPathArc(void* ctx, D2D1_SIZE_F size, D2D1_POINT_2F endPoint,
    float sweepAngle, D2D1_SWEEP_DIRECTION sweepDirection)
{
	D2DPathContext* pathContext = reinterpret_cast<D2DPathContext*>(ctx);

	D2D1_ARC_SEGMENT seg;
	seg.rotationAngle = sweepAngle;
	seg.arcSize = D2D1_ARC_SIZE::D2D1_ARC_SIZE_LARGE;
	seg.point = endPoint;
	seg.size = size;
	seg.sweepDirection = sweepDirection;

	if (!pathContext->isOpen)
	{
		pathContext->sink->BeginFigure(endPoint, D2D1_FIGURE_BEGIN_HOLLOW);
		pathContext->isOpen = true;
	}

	pathContext->sink->AddArc(&seg);
}

static void DrawPath(void* pathCtx, D2D1_COLOR_F strokeColor, float strokeWidth, D2D1_DASH_STYLE dashStyle)
{
	D2DPathContext* pathContext = reinterpret_cast<D2DPathContext*>(pathCtx);
	D2DContext* context = pathContext->d2context;

	ID2D1Factory* factory = context->factory;
	ID2D1RenderTarget* renderTarget = context->renderTarget;

	ID2D1SolidColorBrush* strokeBrush = NULL;
	renderTarget->CreateSolidColorBrush(strokeColor, &strokeBrush);

	ID2D1StrokeStyle *strokeStyle = NULL;

	if (dashStyle != D2D1_DASH_STYLE_SOLID)
	{
		factory->CreateStrokeStyle(D2D1::StrokeStyleProperties(
          D2D1_CAP_STYLE_FLAT,
          D2D1_CAP_STYLE_FLAT,
          D2D1_CAP_STYLE_ROUND,
          D2D1_LINE_JOIN_MITER,
          10.0f,
					dashStyle,
          0.0f), NULL, 0, &strokeStyle);
	}

	renderTarget->DrawGeometry(pathContext->path, strokeBrush, strokeWidth, strokeStyle);

	SafeRelease(&strokeBrush);
	SafeRelease(&strokeStyle);
}

static void FillPathD(void* pathCtx, D2D1_COLOR_F fillColor)
{
	D2DPathContext* pathContext = reinterpret_cast<D2DPathContext*>(pathCtx);
	ID2D1RenderTarget* renderTarget = pathContext->d2context->renderTarget;

	if (fillColor.a > 0)
	{
		ID2D1SolidColorBrush* fillBrush = NULL;
		renderTarget->CreateSolidColorBrush(fillColor, &fillBrush);

		renderTarget->FillGeometry(pathContext->path, fillBrush);

		SafeRelease(&fillBrush);
	}
}

static void DrawBeziers(void* ctx, D2D1_BEZIER_SEGMENT* bezierSegments, uint32_t count,
								 D2D1_COLOR_F strokeColor, float strokeWidth,
								 D2D1_DASH_STYLE dashStyle)
{
	if (count <= 0) return;

	D2DContext* context = reinterpret_cast<D2DContext*>(ctx);

	ID2D1Factory* factory = context->factory;
	ID2D1RenderTarget* renderTarget = context->renderTarget;

	ID2D1PathGeometry* path;
	ID2D1GeometrySink* sink;

	context->factory->CreatePathGeometry(&path);
	path->Open(&sink);
	//sink->SetFillMode(D2D1_FILL_MODE_WINDING);
	sink->BeginFigure(bezierSegments[0].point1, D2D1_FIGURE_BEGIN_FILLED);

	sink->AddBeziers(bezierSegments, count);

	sink->EndFigure(D2D1_FIGURE_END_OPEN);
	sink->Close();

	ID2D1SolidColorBrush* strokeBrush = NULL;
	renderTarget->CreateSolidColorBrush(strokeColor, &strokeBrush);

	ID2D1StrokeStyle *strokeStyle = NULL;

	if (dashStyle != D2D1_DASH_STYLE_SOLID)
	{
		factory->CreateStrokeStyle(D2D1::StrokeStyleProperties(
          D2D1_CAP_STYLE_FLAT,
          D2D1_CAP_STYLE_FLAT,
          D2D1_CAP_STYLE_ROUND,
          D2D1_LINE_JOIN_MITER,
          10.0f,
					dashStyle,
          0.0f), NULL, 0, &strokeStyle);
	}

	renderTarget->DrawGeometry(path, strokeBrush, strokeWidth, strokeStyle);

	SafeRelease(&strokeBrush);
	SafeRelease(&strokeStyle);

	SafeRelease(&sink);
	SafeRelease(&path);
}


static void DrawPolygonWithBrush(void* ctx, D2D1_POINT_2F* points, uint32_t count,
	D2D1_COLOR_F strokeColor, float strokeWidth, D2D1_DASH_STYLE dashStyle, void* brushHandle)
{
	RetrieveContext(ctx);
	HRESULT hr;

	ID2D1PathGeometry* path = NULL;
	hr = context->factory->CreatePathGeometry(&path);

	ID2D1GeometrySink* sink = NULL;
	hr = path->Open(&sink);

	//sink->SetFillMode(D2D1_FILL_MODE_WINDING);
	sink->BeginFigure(points[0], D2D1_FIGURE_BEGIN_FILLED);
	sink->AddLines(points + 1, count - 1);
	sink->EndFigure(D2D1_FIGURE_END_CLOSED);
	hr = sink->Close();

	ID2D1Factory* factory = context->factory;
	ID2D1RenderTarget* renderTarget = context->renderTarget;

	ID2D1Brush* brush = NULL;
	if (brushHandle != NULL) {
		brush = reinterpret_cast<ID2D1Brush*>(brushHandle);
		renderTarget->FillGeometry(path, brush);
	}

	if (strokeColor.a > 0 && strokeWidth > 0)
	{
		ID2D1SolidColorBrush* strokeBrush = NULL;
		hr = renderTarget->CreateSolidColorBrush(strokeColor, &strokeBrush);

		ID2D1StrokeStyle *strokeStyle = NULL;

		if (dashStyle != D2D1_DASH_STYLE_SOLID)
		{
			factory->CreateStrokeStyle(D2D1::StrokeStyleProperties(
				D2D1_CAP_STYLE_FLAT,
				D2D1_CAP_STYLE_FLAT,
				D2D1_CAP_STYLE_ROUND,
				D2D1_LINE_JOIN_MITER,
				10.0f,
				dashStyle,
				0.0f), NULL, 0, &strokeStyle);
		}

		renderTarget->DrawGeometry(path, strokeBrush, strokeWidth, strokeStyle);

		SafeRelease(&strokeBrush);
		SafeRelease(&strokeStyle);
	}

	SafeRelease(&sink);
	SafeRelease(&path);

}

static void DrawPolygon(void* ctx, D2D1_POINT_2F* points, uint32_t count,
	D2D1_COLOR_F strokeColor, float strokeWidth, D2D1_DASH_STYLE dashStyle, D2D1_COLOR_F fillColor)
{
	RetrieveContext(ctx);
	ID2D1SolidColorBrush* fillBrush = NULL;

	if (fillColor.a > 0)
	{
		ID2D1RenderTarget* renderTarget = context->renderTarget;
		renderTarget->CreateSolidColorBrush(fillColor, &fillBrush);
	}

	DrawPolygonWithBrush(ctx, points, count, strokeColor, strokeWidth, dashStyle, fillBrush);

	if (fillBrush != NULL) {
		SafeRelease(&fillBrush);
	}
}

static void FillPathWithBrush(void* ctx, void* brushHandle)
{
	D2DPathContext* pathContext = reinterpret_cast<D2DPathContext*>(ctx);
	ID2D1Brush* brush = reinterpret_cast<ID2D1Brush*>(brushHandle);
	D2DContext* context = pathContext->d2context;

	context->renderTarget->FillGeometry(pathContext->path, brush);
}

static void FillGeometryWithBrush(void* ctx, void* geoHandle, void* brushHandle, void* opacityBrushHandle)
{
	RetrieveContext(ctx);

	ID2D1Geometry* geometry = reinterpret_cast<ID2D1Geometry*>(geoHandle);
	ID2D1Brush* brush = reinterpret_cast<ID2D1Brush*>(brushHandle);
	ID2D1Brush* opacityBrush = reinterpret_cast<ID2D1Brush*>(opacityBrushHandle);

	context->renderTarget->FillGeometry(geometry, brush, opacityBrush);
}

static bool PathFillContainsPoint(void* pathCtx, D2D1_POINT_2F point)
{
	D2DPathContext* pathContext = reinterpret_cast<D2DPathContext*>(pathCtx);

	BOOL contain = FALSE;
	pathContext->path->FillContainsPoint(point, NULL, &contain);

	return contain == TRUE;
}

static bool PathStrokeContainsPoint(void* pathCtx, D2D1_POINT_2F point, float strokeWidth, D2D1_DASH_STYLE dashStyle)
{
	D2DPathContext* pathContext = reinterpret_cast<D2DPathContext*>(pathCtx);

	ID2D1Factory* factory = pathContext->d2context->factory;
	ID2D1StrokeStyle *strokeStyle = NULL;

	if (dashStyle != D2D1_DASH_STYLE_SOLID)
	{
		factory->CreateStrokeStyle(D2D1::StrokeStyleProperties(
			D2D1_CAP_STYLE_FLAT,
			D2D1_CAP_STYLE_FLAT,
			D2D1_CAP_STYLE_ROUND,
			D2D1_LINE_JOIN_MITER,
			10.0f,
			dashStyle,
			0.0f), NULL, 0, &strokeStyle);
	}

	BOOL contain = FALSE;
	pathContext->path->StrokeContainsPoint(point, strokeWidth, strokeStyle, NULL, &contain);

	SafeRelease(&strokeStyle);

	return contain == TRUE;
}

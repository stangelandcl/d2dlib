namespace Direct2D {


ref struct D2DDevice;
ref struct D2DBitmapGraphics;
static D2DBitmapGraphics^ CreateNewD2DBitmapGraphics(void*);
static void* BmpGraphicsHandle(D2DBitmapGraphics^);

static cli::array<uint8_t>^ ToUtf16(String^ s)
{
        auto bytes = Encoding::Unicode->GetByteCount(s);
        auto n = gcnew cli::array<uint8_t>(bytes + 2);
        Encoding::Unicode->GetBytes(s, 0, s->Length, n, 0);
        return n;
}



public ref struct D2DObject : IDisposable
{
        D2DObject(void* handle)
        {
                this->Handle = handle;
        }

        ~D2DObject()
        {
                if (Handle)
                        ReleaseObject(Handle);
                GC::SuppressFinalize(this);
        }
        !D2DObject()
        {
                if(Handle)
                        ReleaseObject(Handle);
        }

internal:
        void* Handle;
};

public ref struct D2DPen : D2DObject
{
public:
        property D2DColor Color { D2DColor get() { return color; } }


        property D2DDashStyle DashStyle { D2DDashStyle get() { return dashStyle; } }

	D2DPen(void* handle, D2DColor color, D2DDashStyle dashStyle)
                : D2DObject(handle)
        {
                this->color = color;
                this->dashStyle = dashStyle;
        }
private:
        D2DColor color;
        D2DDashStyle dashStyle;

};

public ref struct D2DBrush abstract : D2DObject
{
internal:
        D2DBrush(void* handle) : D2DObject(handle) { }
};

public ref struct D2DSolidColorBrush : D2DBrush
{
        property D2DColor Color
        {
                D2DColor get()
                {
                        return color;
                }
                void set(D2DColor value)
                {
                        SetSolidColorBrushColor(Handle, (D2D1_COLOR_F*)&value);
                }
        }

internal:
        D2DSolidColorBrush(void* handle, D2DColor color)
                : D2DBrush(handle)
        {
                this->color = color;
        }
private:
        D2DColor color;
};

public ref struct D2DRadialGradientBrush : D2DBrush
{
        property cli::array<D2DGradientStop>^ GradientStops;

 internal:
        D2DRadialGradientBrush(void* handle, cli::array<D2DGradientStop>^ gradientStops)
                 : D2DBrush(handle)
        {
                GradientStops = gradientStops;
        }
};

public ref struct D2DGeometry : D2DObject
{
internal:
        property void* DeviceHandle;

        D2DGeometry(void* deviceHandle, void* geoHandle)
                : D2DObject(geoHandle)
        {
                this->DeviceHandle = deviceHandle;
        }
public:
        void FillGeometry(D2DBrush^ brush, [Optional] D2DBrush^ opacityBrush)  { }
};

public ref struct D2DRectangleGeometry : D2DGeometry
{
internal:
        D2DRectangleGeometry(void* deviceHandle, void* geoHandle)
                : D2DGeometry(deviceHandle, geoHandle) { }
};

public ref struct D2DPathGeometry : D2DGeometry
{
internal:
        D2DPathGeometry(void* deviceHandle, void* pathHandle)
                : D2DGeometry(deviceHandle, pathHandle) { }

public:
        void AddLines(cli::array<D2DPoint>^ points)
        {
                if(!points->Length)
                        return;

                pin_ptr<D2DPoint> p = &points[0];
                AddPathLines(Handle, (D2D1_POINT_2F*)p, (uint32_t)points->Length);
        }

        void AddBeziers(cli::array<D2DBezierSegment>^ bezierSegments)
        {
                if(!bezierSegments->Length)
                        return;

                pin_ptr<D2DBezierSegment> p = &bezierSegments[0];
                AddPathBeziers(Handle, (D2D1_BEZIER_SEGMENT*)p,
                    (int32_t)bezierSegments->Length);
        }

        void AddEllipse(D2DEllipse ellipse)
        {
                AddPathEllipse(Handle, (D2D1_ELLIPSE*)&ellipse);
        }

        void AddArc(D2DSize size, D2DPoint endPoint, float sweepAngle,
            D2D1_SWEEP_DIRECTION sweepDirection)
        {
                AddPathArc(Handle, *(D2D1_SIZE_F*)&size, *(D2D1_POINT_2F*)&endPoint,
                    sweepAngle, (::D2D1_SWEEP_DIRECTION)sweepDirection);
        }

        void AddArc(D2DSize size, D2DPoint	endPoint, float sweepAngle)
        {
                AddArc(size, endPoint, sweepAngle,
                    D2D1_SWEEP_DIRECTION::D2D1_SWEEP_DIRECTION_CLOCKWISE);
        }


        bool FillContainsPoint(D2DPoint point)
        {
                return PathFillContainsPoint(Handle, *(D2D1_POINT_2F*)&point);
        }

        bool StrokeContainsPoint(D2DPoint point, float width, D2DDashStyle dashStyle)
        {
                return PathStrokeContainsPoint(Handle, *(D2D1_POINT_2F*)&point,
                    width, (D2D1_DASH_STYLE)dashStyle);
        }
        bool StrokeContainsPoint(D2DPoint point, float width)
        {
                return StrokeContainsPoint(point, width, D2DDashStyle::Solid);
        }

        bool StrokeContainsPoint(D2DPoint point)
        {
                return StrokeContainsPoint(point, 1, D2DDashStyle::Solid);
        }

        void ClosePath()
        {
                ::ClosePath(Handle);
        }

        virtual ~D2DPathGeometry() override
        {
                DestroyPathGeometry(Handle);
        }
};

public ref struct  D2DBitmap : D2DObject
{
        D2DSize size;
        property D2DSize Size { D2DSize get() {return size;}}
        property float Height { float get() { return Size.height; } }
        property float Width { float get() { return Size.width; } }

internal:
        D2DBitmap(void* handle)
                : D2DObject(handle)
         {
                 auto s = GetBitmapSize(handle);
                 size = *(D2DSize*)&s;
         }
};


public ref struct D2DDevice: IDisposable
{
internal:
        void* Handle;


        D2DDevice(void* deviceHandle)
        {
                this->Handle = deviceHandle;
        }
public:
	static D2DDevice^ FromHwnd(void* hwnd)
        {
                void* contextHandle = CreateContext((HWND)hwnd);
                return gcnew D2DDevice(contextHandle);
        }

	void Resize()
        {
                if (Handle) ResizeContext(Handle);
        }

	D2DPen^ CreatePen(D2DColor color, D2DDashStyle dashStyle)
        {
                void* handle = ::CreatePenStroke(Handle, *(D2D1_COLOR_F*)&color,
                    (D2D1_DASH_STYLE)dashStyle);
                return !handle ? nullptr : gcnew D2DPen(handle, color, dashStyle);
        }

	D2DSolidColorBrush^ CreateSolidColorBrush(D2DColor color)
        {
                void* handle = ::CreateSolidColorBrush(Handle, *(D2D1_COLOR_F*)&color);
                return !handle ? nullptr : gcnew D2DSolidColorBrush(handle, color);
        }

	D2DRadialGradientBrush^ CreateRadialGradientBrush(D2DPoint origin, D2DPoint offset,
            float radiusX, float radiusY, cli::array<D2DGradientStop>^ gradientStops)
        {
                pin_ptr<D2DGradientStop> p = &gradientStops[0];
                void* handle = ::CreateRadialGradientBrush(Handle,
                    *(D2D1_POINT_2F*)&origin, *(D2D1_POINT_2F*)&offset,
                    radiusX, radiusY, (D2D1_GRADIENT_STOP *)p, (uint32_t)gradientStops->Length);

                return gcnew D2DRadialGradientBrush(handle, gradientStops);
        }

	D2DRectangleGeometry^ CreateRectangleGeometry(float width, float height)
        {
                auto rect = D2DRect(0, 0, width, height);
                return CreateRectangleGeometry(rect);
        }

        D2DRectangleGeometry^ CreateRectangleGeometry(D2DRect% rect)
        {
                pin_ptr<D2DRect> rp = &rect;
                D2D1_RECT_F* r = (D2D1_RECT_F*)rp;
                void* rectHandle = ::CreateRectangleGeometry(Handle, *r);
                return gcnew D2DRectangleGeometry(Handle, rectHandle);
        }

	D2DPathGeometry^ CreatePathGeometry()
        {
                void* geoHandle = ::CreatePathGeometry(Handle);
                return gcnew D2DPathGeometry(Handle, geoHandle);
        }

        D2DBitmap^ LoadBitmap(cli::array<uint8_t>^ buffer)
        {
                return this->LoadBitmap(buffer, 0, (uint32_t)buffer->Length);
        }

	D2DBitmap^ LoadBitmap(cli::array<uint8_t>^ buffer, uint32_t offset, uint32_t length)
        {
                pin_ptr<uint8_t> b = &buffer[0];
                auto bitmapHandle = ::CreateBitmapFromBytes(Handle, b, offset, length);
                return bitmapHandle ? gcnew D2DBitmap(bitmapHandle) : nullptr;
        }

	D2DBitmap^ LoadBitmap(String^ filepath)
        {
                auto n = ToUtf16(filepath);
                pin_ptr<uint8_t> p = &n[0];
                auto bitmapHandle = ::CreateBitmapFromFile(Handle, (LPCWSTR)p);
                return bitmapHandle ? gcnew D2DBitmap(bitmapHandle) : nullptr;
        }

	D2DBitmap^ CreateBitmapFromMemory(uint32_t width, uint32_t height,
            uint32_t stride, void* buffer, uint32_t offset, uint32_t length)
        {
                void* d2dbmp = ::CreateBitmapFromMemory(Handle, width, height,
                    stride, (uint8_t*)buffer, offset, length);
                return !d2dbmp ? nullptr : gcnew D2DBitmap(d2dbmp);
        }

        D2DBitmap^ CreateBitmapFromGDIBitmap(System::Drawing::Bitmap^ bmp)
        {
                bool useAlphaChannel =
                    (bmp->PixelFormat & System::Drawing::Imaging::PixelFormat::Alpha)
                    == System::Drawing::Imaging::PixelFormat::Alpha;

                return this->CreateBitmapFromGDIBitmap(bmp, useAlphaChannel);
        }

        D2DBitmap^ CreateBitmapFromGDIBitmap(System::Drawing::Bitmap^ bmp, bool useAlphaChannel)
        {
                void* d2dbmp = ::CreateBitmapFromHBitmap(Handle,
                    (HBITMAP)(void*)bmp->GetHbitmap(), useAlphaChannel);
                return !d2dbmp ? nullptr : gcnew D2DBitmap(d2dbmp);
        }

	D2DBitmapGraphics^ CreateBitmapGraphics()
        {
                return CreateBitmapGraphics(D2DSize::Empty);
        }

        D2DBitmapGraphics^ CreateBitmapGraphics(float width, float height)
        {
                return CreateBitmapGraphics(D2DSize(width, height));
        }

        D2DBitmapGraphics^ CreateBitmapGraphics(D2DSize size)
        {
                void* bitmapRenderTargetHandle = CreateBitmapRenderTarget(Handle,
                    *(D2D1_SIZE_F*)&size);
                return !bitmapRenderTargetHandle ? nullptr
                    : CreateNewD2DBitmapGraphics(bitmapRenderTargetHandle);
        }

	~D2DDevice()
        {
                DestroyContext(Handle);
                GC::SuppressFinalize(this);
        }

        !D2DDevice()
        {
                DestroyContext(Handle);
        }

};


public ref struct D2DGraphics
{
internal:
        void* DeviceHandle;
public:
        D2DDevice^ device;
        property D2DDevice^ Device { D2DDevice^ get() { return device; }}

        D2DGraphics(D2DDevice^ context)
                : D2DGraphics(context->Handle)
        {
                device = context;
        }

        D2DGraphics(void* device)
        {
                DeviceHandle = device;
        }

        void BeginRender()
        {
                ::BeginRender(DeviceHandle);
        }

        void BeginRender(D2DColor color)
        {
                ::BeginRenderWithBackgroundColor(DeviceHandle, *(D2D1_COLOR_F*)&color);
        }

        void BeginRender(D2DBitmap^ bitmap)
        {
                ::BeginRenderWithBackgroundBitmap(DeviceHandle, bitmap->Handle);
        }

        void EndRender()
        {
                ::EndRender(DeviceHandle);
        }

        void Flush()
        {
                ::Flush(DeviceHandle);
        }


        property bool Antialias
        {
                bool get() { return antialias; }
                void set(bool value)
                {
                        if (antialias != value)
                        {
                                SetContextProperties(DeviceHandle,
                                    (D2D1_ANTIALIAS_MODE)(
                                            value ? D2DAntialiasMode::PerPrimitive : D2DAntialiasMode::Aliased));

                                antialias = value;
                        }
                }
        }

        void DrawLine(float x1, float y1, float x2, float y2, D2DColor color,
            float weight, D2DDashStyle dashStyle)
        {
                DrawLine(D2DPoint(x1, y1), D2DPoint(x2, y2), color, weight, dashStyle);
        }

         void DrawLine(float x1, float y1, float x2, float y2, D2DColor color,
            float weight)
        {
                DrawLine(x1, y1, x2, y2, color, weight, D2DDashStyle::Solid);
        }

        void DrawLine(float x1, float y1, float x2, float y2, D2DColor color)
        {
                DrawLine(x1, y1, x2, y2, color, 1, D2DDashStyle::Solid);
        }

        void DrawLine(D2DPoint start, D2DPoint end, D2DColor color,
            float weight, D2DDashStyle dashStyle)
        {
                ::DrawLine(DeviceHandle, *(D2D1_POINT_2F*)&start,
                    *(D2D1_POINT_2F*)&end,
                    *(D2D1_COLOR_F*)&color, weight, (D2D1_DASH_STYLE)dashStyle);
        }
        void DrawLine(D2DPoint start, D2DPoint end, D2DColor color, float weight)
        {
                DrawLine(start, end, color, weight, D2DDashStyle::Solid);
        }
        void DrawLine(D2DPoint start, D2DPoint end, D2DColor color)
        {
                DrawLine(start, end, color, 1, D2DDashStyle::Solid);
        }


        void DrawLines(cli::array<D2DPoint>^ points, D2DColor color,
            float weight, D2DDashStyle dashStyle)
        {
                if(!points->Length)
                        return;

                pin_ptr<D2DPoint> p = &points[0];
                ::DrawLines(DeviceHandle, (D2D1_POINT_2F*)p, (uint32_t)points->Length,
                    *(D2D1_COLOR_F*)&color,
                    weight, (D2D1_DASH_STYLE)dashStyle);
        }
        void DrawLines(cli::array<D2DPoint>^ points, D2DColor color,
            float weight)
        {
                DrawLines(points, color, weight, D2DDashStyle::Solid);
        }

        void DrawLines(cli::array<D2DPoint>^ points, D2DColor color)
        {
                DrawLines(points, color, 1, D2DDashStyle::Solid);
        }



        void DrawEllipse(float x, float y, float width, float height, D2DColor color,
            float weight, D2DDashStyle dashStyle)
        {
                auto ellipse = D2DEllipse(x, y, width / 2.f, height / 2.f);
                ellipse.origin.x += ellipse.radiusX;
                ellipse.origin.y += ellipse.radiusY;

                DrawEllipse(ellipse, color, weight, dashStyle);
        }

        void DrawEllipse(float x, float y, float width, float height, D2DColor color,
            float weight)
        {
                DrawEllipse(x, y, width, height, color, weight, D2DDashStyle::Solid);
        }
        void DrawEllipse(float x, float y, float width, float height, D2DColor color)
        {
                DrawEllipse(x, y, width, height, color, 1, D2DDashStyle::Solid);
        }


        void DrawEllipse(D2DPoint origin, D2DSize radial, D2DColor color,
            float weight, D2DDashStyle dashStyle)
        {
                auto ellipse = D2DEllipse(origin, radial);
                DrawEllipse(ellipse, color, weight, dashStyle);
        }

        void DrawEllipse(D2DPoint origin, D2DSize radial, D2DColor color, float weight)
        {
                DrawEllipse(origin, radial, color, weight, D2DDashStyle::Solid);
        }

        void DrawEllipse(D2DPoint origin, D2DSize radial, D2DColor color)
        {
                DrawEllipse(origin, radial, color, 1, D2DDashStyle::Solid);
        }

        void DrawEllipse(D2DPoint origin, float radialX, float radialY, D2DColor color,
            float weight, D2DDashStyle dashStyle)
        {
                auto ellipse = D2DEllipse(origin, radialX, radialY);
                DrawEllipse(ellipse, color, weight, dashStyle);
        }

        void DrawEllipse(D2DPoint origin, float radialX, float radialY, D2DColor color,
            float weight)
        {
                DrawEllipse(origin, radialX, radialY, color, weight, D2DDashStyle::Solid);
        }

        void DrawEllipse(D2DPoint origin, float radialX, float radialY, D2DColor color)
        {
                DrawEllipse(origin, radialX, radialY, color, 1, D2DDashStyle::Solid);
        }

        void DrawEllipse(D2DEllipse ellipse, D2DColor color, float weight, D2DDashStyle dashStyle)
        {
                ::DrawEllipse(DeviceHandle, (D2D1_ELLIPSE *)&ellipse,
                    *(D2D1_COLOR_F*)&color, weight, (D2D1_DASH_STYLE)dashStyle);
        }

        void DrawEllipse(D2DEllipse ellipse, D2DColor color, float weight)
        {
                DrawEllipse(ellipse, color, weight, D2DDashStyle::Solid);
        }

        void DrawEllipse(D2DEllipse ellipse, D2DColor color)
        {
                DrawEllipse(ellipse, color, 1, D2DDashStyle::Solid);
        }

        void FillEllipse(D2DPoint p, float radial, D2DColor color)
        {
                FillEllipse(p, radial, radial, color);
        }

        void FillEllipse(D2DPoint p, float w, float h, D2DColor color)
        {
                D2DEllipse ellipse = D2DEllipse(p, w / 2, h / 2);
                ellipse.origin.x += ellipse.radiusX;
                ellipse.origin.y += ellipse.radiusY;

                FillEllipse(ellipse, color);
        }

        void FillEllipse(float x, float y, float radial, D2DColor color)
        {
                FillEllipse(D2DPoint(x, y), radial, radial, color);
        }

        void FillEllipse(float x, float y, float w, float h, D2DColor color)
        {
                FillEllipse(D2DPoint(x, y), w, h, color);
        }

        void FillEllipse(D2DEllipse ellipse, D2DColor color)
        {
                ::FillEllipse(DeviceHandle, (D2D1_ELLIPSE *)&ellipse,
                    *(D2D1_COLOR_F*)&color);
        }

	void FillEllipse(D2DEllipse ellipse, D2DBrush^ brush)
        {
                ::FillEllipseWithBrush(DeviceHandle,
                    (D2D1_ELLIPSE *)&ellipse, brush->Handle);
        }

        void DrawBeziers(cli::array<D2DBezierSegment>^ bezierSegments,
            D2DColor strokeColor, float strokeWidth,
            D2DDashStyle dashStyle)
        {
                if(!bezierSegments->Length)
                        return;

                pin_ptr<D2DBezierSegment> p = &bezierSegments[0];
                ::DrawBeziers(DeviceHandle, (D2D1_BEZIER_SEGMENT *)p,
                    (uint32_t)bezierSegments->Length,
                    *(D2D1_COLOR_F*)&strokeColor,
                    strokeWidth, (D2D1_DASH_STYLE)dashStyle);
        }

        void DrawPolygon(cli::array<D2DPoint>^ points, D2DColor strokeColor,
            D2DDashStyle dashStyle)
        {
                DrawPolygon(points, strokeColor, 1.f, dashStyle, D2DColor::Transparent);
        }

        void DrawPolygon(cli::array<D2DPoint>^ points, D2DColor strokeColor,
            float strokeWidth, D2DDashStyle dashStyle, D2DColor fillColor)
        {
                if(!points->Length)
                        return;

                pin_ptr<D2DPoint> p = &points[0];
                ::DrawPolygon(DeviceHandle,
                    (D2D1_POINT_2F *)p, (uint32_t)points->Length,
                    *(D2D1_COLOR_F*)&strokeColor, strokeWidth,
                    (D2D1_DASH_STYLE)dashStyle,
                    *(D2D1_COLOR_F*)&fillColor);
        }

        void DrawPolygon(cli::array<D2DPoint>^ points, D2DColor strokeColor,
            float strokeWidth, D2DDashStyle dashStyle, D2DBrush^ fillBrush)
        {
                if(!points->Length)
                        return;

                pin_ptr<D2DPoint> p = &points[0];

                ::DrawPolygonWithBrush(DeviceHandle, (D2D1_POINT_2F *)p, (uint32_t)points->Length,
                    *(D2D1_COLOR_F*)&strokeColor, strokeWidth,
                    (D2D1_DASH_STYLE)dashStyle, fillBrush->Handle);
        }

        void FillPolygon(cli::array<D2DPoint>^ points, D2DColor fillColor)
        {
                DrawPolygon(points, D2DColor::Transparent, 0, D2DDashStyle::Solid, fillColor);
        }

        void FillPolygon(cli::array<D2DPoint>^ points, D2DBrush^ brush)
        {
                if(!points->Length)
                        return;

                pin_ptr<D2DPoint> p = &points[0];
                auto c = D2DColor::Transparent;
                DrawPolygonWithBrush(DeviceHandle, (D2D1_POINT_2F*)p, (uint32_t)points->Length,
                    *(D2D1_COLOR_F*)&c, 0,
                    (D2D1_DASH_STYLE)D2DDashStyle::Solid, brush->Handle);
        }
#if 0
        void TestDraw()
        {
                ::TestDraw(DeviceHandle);
        }
#endif
        void PushClip(D2DRect rect, D2D1_ANTIALIAS_MODE antialias)
        {
                ::PushClip(DeviceHandle, (D2D1_RECT_F*)&rect, antialias);
        }

        void PopClip()
        {
                ::PopClip(DeviceHandle);
        }

        void PushTransform()
        {
                ::PushTransform(DeviceHandle);
        }

        void PopTransform()
        {
                ::PopTransform(DeviceHandle);
        }

        void ResetTransform()
        {
                ::ResetTransform(DeviceHandle);
        }

        void RotateTransform(float angle)
        {
                RotateTransform(angle, D2DPoint());
        }

        void RotateTransform(float angle, D2DPoint center)
        {
                ::RotateTransform(DeviceHandle, angle, *(D2D1_POINT_2F*)&center);
        }

        void TranslateTransform(float x, float y)
        {
                ::TranslateTransform(DeviceHandle, x, y);
        }

        void ScaleTransform(float sx, float sy, [Optional] D2DPoint center)
        {
                ::ScaleTransform(DeviceHandle, sx, sy, *(D2D1_POINT_2F*)&center);
        }

        void SkewTransform(float angleX, float angleY, [Optional] D2DPoint center)
        {
                ::SkewTransform(DeviceHandle, angleX, angleY, *(D2D1_POINT_2F*)&center);
        }

        void DrawRectangle(float x, float y, float w, float h, D2DColor color, float strokeWidth,
            D2DDashStyle dashStyle)
        {
                D2DRect rect = D2DRect(x, y, w, h);
                ::DrawRectangle(DeviceHandle, (D2D1_RECT_F*)&rect,
                    *(D2D1_COLOR_F*)&color,
                    strokeWidth, (D2D1_DASH_STYLE)dashStyle);
        }

        void DrawRectangle(D2DRect rect, D2DColor color, float strokeWidth,
            D2DDashStyle dashStyle)
        {
                ::DrawRectangle(DeviceHandle, (D2D1_RECT_F*)&rect,
                    *(D2D1_COLOR_F*)&color, strokeWidth, (D2D1_DASH_STYLE)dashStyle);
        }

        void DrawRectangle(D2DPoint origin, D2DSize size, D2DColor color, float strokeWidth,
            D2DDashStyle dashStyle)
        {
                DrawRectangle(D2DRect(origin, size), color, strokeWidth, dashStyle);
        }

        void FillRectangle(float x, float y, float width, float height, D2DColor color)
        {
                auto rect = D2DRect(x, y, width, height);
                FillRectangle(rect, color);
        }

        void FillRectangle(D2DPoint origin, D2DSize size, D2DColor color)
        {
                FillRectangle(D2DRect(origin, size), color);
        }

        void FillRectangle(D2DRect rect, D2DColor color)
        {
                ::FillRectangle(DeviceHandle, (D2D1_RECT_F*)&rect,
                    *(D2D1_COLOR_F*)&color);
        }

        void DrawBitmap(D2DBitmap^ bitmap, D2DRect destRect, float opacity,
            D2DBitmapInterpolationMode interpolationMode)
        {
                auto srcRect = D2DRect(0, 0, bitmap->Width, bitmap->Height);
                DrawBitmap(bitmap, destRect, srcRect, opacity, interpolationMode);
        }

        void DrawBitmap(D2DBitmap^ bitmap, D2DRect destRect, D2DRect srcRect, float opacity,
            D2DBitmapInterpolationMode interpolationMode)
        {
                DrawD2DBitmap(DeviceHandle, bitmap->Handle, (D2D1_RECT_F*)&destRect,
                    (D2D1_RECT_F*)&srcRect,
                    opacity, (D2D1_BITMAP_INTERPOLATION_MODE)interpolationMode);
        }

        void DrawBitmap(D2DBitmapGraphics^ bg, D2DRect rect, float opacity,
            D2DBitmapInterpolationMode interpolationMode)
        {
                ::DrawBitmapRenderTarget(DeviceHandle, BmpGraphicsHandle(bg), (D2D1_RECT_F *)&rect,
                    opacity, (D2D1_BITMAP_INTERPOLATION_MODE)interpolationMode);
        }

        void DrawBitmap(D2DBitmapGraphics^ bg, float width, float height, float opacity,
            D2DBitmapInterpolationMode interpolationMode)
        {
                DrawBitmap(bg, D2DRect(0, 0, width, height), opacity, interpolationMode);
        }

        void DrawGDIBitmap(void* hbitmap, D2DRect rect, D2DRect srcRect,
            float opacity, bool alpha,
            D2DBitmapInterpolationMode interpolationMode)
        {
                ::DrawGDIBitmapRect(DeviceHandle,
                    (HBITMAP)hbitmap, (D2D1_RECT_F *)&rect, (D2D1_RECT_F *)&srcRect,
                    opacity,
                    alpha, (D2D1_BITMAP_INTERPOLATION_MODE)interpolationMode);
        }

        void DrawTextCenter(String^ text, D2DColor color, String^ fontName,
            float fontSize, D2DRect rect)
        {
                DrawText(text, color, fontName, fontSize, rect,
                    DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_CENTER,
                    DWRITE_PARAGRAPH_ALIGNMENT::DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        }

        void DrawText(String^ text, D2DColor color, String^ fontName, float fontSize, D2DRect rect,
            DWRITE_TEXT_ALIGNMENT halign,DWRITE_PARAGRAPH_ALIGNMENT valign)
        {
                auto textbytes = ToUtf16(text);
                auto name = ToUtf16(fontName);
                pin_ptr<uint8_t> b = &name[0];
                pin_ptr<uint8_t> tp = &textbytes[0];
                ::DrawString(DeviceHandle, (LPCWSTR)tp, *(D2D1_COLOR_F*)&color,
                    (LPCWSTR)b, fontSize, (D2D1_RECT_F *)&rect,
                    (::DWRITE_TEXT_ALIGNMENT)halign,
                    (::DWRITE_PARAGRAPH_ALIGNMENT)valign);
        }

        void DrawText(String^ text, D2DColor color, String^ fontName, float fontSize, D2DRect rect)
        {
                DrawText(text, color, fontName, fontSize, rect,
                    DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_CENTER,
                    DWRITE_PARAGRAPH_ALIGNMENT::DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        }

        void DrawText(String^ text, D2DColor color, System::Drawing::Font^ font, System::Drawing::PointF location)
        {
                auto rect = D2DRect(location.X, location.Y, 9999999, 9999999);
                DrawText(text, color, font->Name, font->Size * 96.f / 72.f, rect);
        }

        void DrawPath(D2DPathGeometry^ path, D2DColor strokeColor,
            float strokeWidth, D2DDashStyle dashStyle)
        {
                ::DrawPath(path->Handle, *(D2D1_COLOR_F*)&strokeColor, strokeWidth,
                    (D2D1_DASH_STYLE)dashStyle);
        }

        void FillPath(D2DPathGeometry^ path, D2DColor fillColor)
        {
                FillPathD(path->Handle, *(D2D1_COLOR_F*)&fillColor);
        }

        void Clear(D2DColor color)
        {
                ::Clear(DeviceHandle, *(D2D1_COLOR_F*)&color);
        }
private:
        bool antialias = true;

};

public ref struct D2DBitmapGraphics : D2DGraphics, IDisposable
{
internal:
         D2DBitmapGraphics(void* handle)
                 : D2DGraphics(handle) { }

public:
         D2DBitmap^ GetBitmap()
         {
                 void* bitmapHandle = GetBitmapRenderTargetBitmap(DeviceHandle);
                 return !bitmapHandle ? nullptr : gcnew D2DBitmap(bitmapHandle);
         }

         ~D2DBitmapGraphics()
         {
                 DestoryBitmapRenderTarget(DeviceHandle);
                 GC::SuppressFinalize(this);
         }

        !D2DBitmapGraphics()
        {
                DestoryBitmapRenderTarget(DeviceHandle);
        }

};


static D2DBitmapGraphics^ CreateNewD2DBitmapGraphics(void* handle)
{
        return gcnew D2DBitmapGraphics(handle);
}

static void* BmpGraphicsHandle(D2DBitmapGraphics^ h)
{
        return h->DeviceHandle;
}


}

namespace Direct2D {

[Serializable]
[StructLayout(LayoutKind::Sequential)]
public value struct D2DColor
{
    float r;
    float g;
    float b;
    float a;

    D2DColor(float r, float g, float b)
        : D2DColor(1, r, g, b)
    {
    }

    D2DColor(float a, float r, float g, float b)
    {
        this->a = a;
        this->r = r;
        this->g = g;
        this->b = b;
    }

    D2DColor(float alpha, D2DColor color)
    {
        this->a = alpha;
        this->r = color.r;
        this->g = color.g;
        this->b = color.b;
    }

    static bool operator ==(D2DColor c1, D2DColor c2)
    {
        return c1.r == c2.r && c1.g == c2.g && c1.b == c2.b && c1.a == c2.a;
    }

    static bool operator !=(D2DColor c1, D2DColor c2)
    {
        return c1.r != c2.r || c1.g != c2.g || c1.b != c2.b || c1.a != c2.a;
    }

    static D2DColor FromGDIColor(System::Drawing::Color gdiColor)
    {
        return D2DColor(gdiColor.A / 255.f, gdiColor.R / 255.f,
            gdiColor.G / 255.f, gdiColor.B / 255.f);
    }

    static System::Drawing::Color ToGDIColor(D2DColor d2color)
    {
        int a = (int)(d2color.a * 255);
        int r = (int)(d2color.r * 255);
        int g = (int)(d2color.g * 255);
        int b = (int)(d2color.b * 255);

        if (a > 255) a = 255; else if (a < 0) a = 0;
        if (r > 255) r = 255; else if (r < 0) r = 0;
        if (g > 255) g = 255; else if (g < 0) g = 0;
        if (b > 255) b = 255; else if (b < 0) b = 0;

        return System::Drawing::Color::FromArgb(a, r, g, b);
    }

    static initonly D2DColor Transparent = D2DColor(0, 0, 0, 0);

    static initonly D2DColor Black = D2DColor(0, 0, 0);
    static initonly D2DColor DimGray = D2DColor::FromGDIColor(System::Drawing::Color::DimGray);
    static initonly D2DColor Gray = D2DColor::FromGDIColor(System::Drawing::Color::Gray);
    static initonly D2DColor DarkGray = D2DColor::FromGDIColor(System::Drawing::Color::DarkGray);
    static initonly D2DColor Silver = D2DColor::FromGDIColor(System::Drawing::Color::Silver);
    static initonly D2DColor GhostWhite = D2DColor::FromGDIColor(System::Drawing::Color::GhostWhite);
    static initonly D2DColor LightGray = D2DColor::FromGDIColor(System::Drawing::Color::LightGray);
    static initonly D2DColor White = D2DColor::FromGDIColor(System::Drawing::Color::White);

    static initonly D2DColor Red = D2DColor::FromGDIColor(System::Drawing::Color::Red);
    static initonly D2DColor DarkRed = D2DColor::FromGDIColor(System::Drawing::Color::DarkRed);
    static initonly D2DColor Coral = D2DColor::FromGDIColor(System::Drawing::Color::Coral);
    static initonly D2DColor LightCoral = D2DColor::FromGDIColor(System::Drawing::Color::LightCoral);

    static initonly D2DColor Beige = D2DColor::FromGDIColor(System::Drawing::Color::Beige);
    static initonly D2DColor Bisque = D2DColor::FromGDIColor(System::Drawing::Color::Bisque);
    static initonly D2DColor LightYellow = D2DColor::FromGDIColor(System::Drawing::Color::LightYellow);
    static initonly D2DColor Yellow = D2DColor::FromGDIColor(System::Drawing::Color::Yellow);
    static initonly D2DColor Gold = D2DColor::FromGDIColor(System::Drawing::Color::Gold);
    static initonly D2DColor Goldenrod = D2DColor::FromGDIColor(System::Drawing::Color::Goldenrod);
    static initonly D2DColor Orange = D2DColor::FromGDIColor(System::Drawing::Color::Orange);
    static initonly D2DColor DarkOrange = D2DColor::FromGDIColor(System::Drawing::Color::DarkOrange);
    static initonly D2DColor BurlyWood = D2DColor::FromGDIColor(System::Drawing::Color::BurlyWood);
    static initonly D2DColor Chocolate = D2DColor::FromGDIColor(System::Drawing::Color::Chocolate);

    static initonly D2DColor LawnGreen = D2DColor::FromGDIColor(System::Drawing::Color::LawnGreen);
    static initonly D2DColor LightGreen = D2DColor::FromGDIColor(System::Drawing::Color::LightGreen);
    static initonly D2DColor Green = D2DColor::FromGDIColor(System::Drawing::Color::Green);
    static initonly D2DColor DarkGreen = D2DColor::FromGDIColor(System::Drawing::Color::DarkGreen);

    static initonly D2DColor AliceBlue = D2DColor::FromGDIColor(System::Drawing::Color::AliceBlue);
    static initonly D2DColor LightBlue = D2DColor::FromGDIColor(System::Drawing::Color::LightBlue);
    static initonly D2DColor Blue = D2DColor::FromGDIColor(System::Drawing::Color::Blue);
    static initonly D2DColor DarkBlue = D2DColor::FromGDIColor(System::Drawing::Color::DarkBlue);
    static initonly D2DColor SkyBlue = D2DColor::FromGDIColor(System::Drawing::Color::SkyBlue);
    static initonly D2DColor SteelBlue = D2DColor::FromGDIColor(System::Drawing::Color::SteelBlue);

    static initonly D2DColor Pink = D2DColor::FromGDIColor(System::Drawing::Color::Pink);
};

[Serializable]
[StructLayout(LayoutKind::Sequential)]
public value struct D2DPoint
{
    float x;
    float y;

    D2DPoint(float x, float y)
    {
        this->x = x;
        this->y = y;
    }

    void Offset(float offx, float offy)
    {
        this->x += offx;
        this->y += offy;
    }

    static initonly D2DPoint Zero = D2DPoint(0, 0);
    static initonly D2DPoint One = D2DPoint(1, 1);

    virtual bool Equals(Object^ obj) override
    {
        if (obj->GetType() != D2DPoint::typeid)
                return false;

        auto p2 = (D2DPoint)obj;

        return x == p2.x && y == p2.y;
    }

    static bool operator==(D2DPoint p1, D2DPoint p2)
    {
        return p1.x == p2.x && p1.y == p2.y;
    }

    static bool operator !=(D2DPoint p1, D2DPoint p2)
    {
        return p1.x != p2.x || p1.y != p2.y;
    }

    static operator D2DPoint(System::Drawing::Point p)
    {
        return D2DPoint(p.X, p.Y);
    }

    static operator D2DPoint(System::Drawing::PointF p)
    {
        return D2DPoint(p.X, p.Y);
    }

    static operator System::Drawing::PointF(D2DPoint p)
    {
        return System::Drawing::PointF(p.x, p.y);
    }

    virtual int GetHashCode() override
    {
        return (int)((this->x * 0xff) + this->y);
    }
};

[Serializable]
[StructLayout(LayoutKind::Sequential)]
public value struct D2DSize
{
    float width;
    float height;

    D2DSize(float width, float height)
    {
        this->width = width;
        this->height = height;
    }

    static initonly D2DSize Empty = D2DSize(0, 0);

    static operator D2DSize(System::Drawing::Size wsize)
    {
        return D2DSize(wsize.Width, wsize.Height);
    }

    static operator D2DSize(System::Drawing::SizeF wsize)
    {
        return D2DSize(wsize.Width, wsize.Height);
    }

    static operator System::Drawing::SizeF(D2DSize s)
    {
        return System::Drawing::SizeF(s.width, s.height);
    }
};


[Serializable]
[StructLayout(LayoutKind::Sequential)]
public value struct D2DRect
{
    float left;
    float top;
    float right;
    float bottom;

    D2DRect(float left, float top, float width, float height)
    {
        this->left = left;
        this->top = top;
        this->right = left + width;
        this->bottom = top + height;
    }

    D2DRect(D2DPoint origin, D2DSize size)
        : D2DRect(origin.x - size.width * 0.5f, origin.y - size.height * 0.5f,
            size.width, size.height)
    { }

    property D2DPoint Location
    {
        D2DPoint get() { return D2DPoint(left, top); }
        void set(D2DPoint value)
        {
            float width = this->right - this->left;
            float height = this->bottom - this->top;
            this->left = value.x;
            this->right = value.x + width;
            this->top = value.y;
            this->bottom = value.y + height;
        }
    }

    property float Width
    {
        float get() { return right - left; }
        void set(float value) { right = left + value; }
    }

    property float Height
    {
        float get() { return bottom - top; }
        void set(float value) { bottom = top + value; }
    }

    void Offset(float x, float y)
    {
        this->left += x;
        this->right += x;
        this->top += y;
        this->bottom += y;
    }

    property float X
    {
        float get() { return this->left; }
        void set(float value)
        {
            float width = this->right - this->left;
            this->left = value;
            this->right = value + width;
        }
    }

    property float Y
    {
        float get() { return this->top; }
        void set(float value)
        {
            float height = this->bottom - this->top;
            this->top = value;
            this->bottom = value + height;
        }
    }

    static operator D2DRect(System::Drawing::Rectangle rect)
    {
        return D2DRect(rect.X, rect.Y, rect.Width, rect.Height);
    }

    static operator D2DRect(System::Drawing::RectangleF rect)
    {
        return D2DRect(rect.X, rect.Y, rect.Width, rect.Height);
    }

    static operator System::Drawing::RectangleF(D2DRect rect)
    {
        return System::Drawing::RectangleF(rect.X, rect.Y, rect.Width, rect.Height);
    }
};



[Serializable]
[StructLayout(LayoutKind::Sequential)]
public value struct D2DEllipse
{
    D2DPoint origin;
    float radiusX;
    float radiusY;

    D2DEllipse(D2DPoint center, float rx, float ry)
    {
        this->origin = center;
        this->radiusX = rx;
        this->radiusY = ry;
    }


    D2DEllipse(D2DPoint center, D2DSize radius)
        : D2DEllipse(center, radius.width, radius.height)
    {
    }

    D2DEllipse(float x, float y, float rx, float ry)
        : D2DEllipse(D2DPoint(x, y), rx, ry)
    {
    }

    property float X{ float get() { return origin.x; } void set(float value) { origin.x = value; } }
    property float Y{ float get() { return origin.y; } void set(float value) { origin.y = value; } }
};

[Serializable]
[StructLayout(LayoutKind::Sequential)]
public value struct D2DBezierSegment
{
    D2DPoint point1;
    D2DPoint point2;
    D2DPoint point3;

    D2DBezierSegment(D2DPoint point1, D2DPoint point2, D2DPoint point3)
    {
        this->point1 = point1;
        this->point2 = point2;
        this->point3 = point3;
    }

    D2DBezierSegment(float x1, float y1, float x2, float y2, float x3, float y3)
    {
        this->point1 = D2DPoint(x1, y1);
        this->point2 = D2DPoint(x2, y2);
        this->point3 = D2DPoint(x3, y3);
    }
};

[Serializable]
[StructLayout(LayoutKind::Sequential)]
public value struct D2DGradientStop
{
    float position;
    D2DColor color;

    D2DGradientStop(float position, D2DColor color)
    {
        this->position = position;
        this->color = color;
    }
};

}

namespace Direct2D {

public enum class WindowsMessages : uint32_t {
        WM_NULL=0,
        WM_CREATE,
        WM_DESTROY,
        WM_MOVE,
        WM_SIZE=5,
        WM_ACTIVATE,
        WM_SETFOCUS,
        WM_KILLFOCUS,
        WM_ENABLE=0xA,
        WM_SETTEXT=0xC,
        WM_GETTEXT,
        WM_GETTEXTLENGTH,
        WM_PAINT,
        WM_CLOSE,
        WM_QUERYENDSESSION,
        WM_QUERYOPEN=0x13,
        WM_ERASEBKGND,
        WM_ENDSESSION=0x16,
        WM_SHOWWINDOW=0x18,
        WM_COPYDATA=0x4A,
        WM_HSCROLL=0x114,
        WM_VSCROLL=0x115,
        WM_CONTEXTMENU=0x7B,
        WM_KEYDOWN=0x100,
        WM_KEYUP,
        WM_MOUSEMOVE=0x200,
        WM_CHAR=0x102,
        WM_LBUTTONDOWN=0x201,
        WM_LBUTTONUP,
        WM_LBUTTONDBLCLK,
        WM_RBUTTONDOWN,
        WM_RBUTTONUP,
        WM_RBUTTONDBLCLK,
        WM_MOUSEWHEEL=0x20E,
        WM_HOTKEY=0x312,
        WM_DRAWCLIPBOARD=0x308,
        WM_CHANGECBCHAIN=0x30D,
        WM_USER=0x400,
};

public ref struct D2DForm : System::Windows::Forms::Form
{
        D2DForm() : System::Windows::Forms::Form()
        {
                timer = Timer { Interval = 10 };
        }

        property D2DDevice^ Device
        {
                D2DDevice^ get()
                {
                        auto hwnd = Handle;
                        if (!device)
                                device = D2DDevice::FromHwnd(hwnd);
                        return device;
		}
        }

        property new D2DBitmap^ BackgroundImage
        {
                D2DBitmap^ get() { return backgroundImage; }
                void set(D2DBitmap^ value)
                {
                        if (backgroundImage != value)
                        {
                                if (backgroundImage)
                                        backgroundImage->Dispose();
                                backgroundImage = value;
                                Invalidate();
                        }
                }
        }


        property bool ShowFPS;
        bool EscapeKeyToClose = true;

        property bool AnimationDraw
        {
                bool get() { return animationDraw; }
                void set(bool value)
                {
                        animationDraw = value;
                        if (!animationDraw && !sceneAnimation && timer->Enabled)
                                timer->Stop();
                }
        }

        new void Invalidate()
        {
                System::Windows::Forms::Form::Invalidate(false);
        }

protected:
        property bool SceneAnimation
        {
                bool get { return sceneAnimation; }
                void set(bool value)
                {
                        sceneAnimation = true;
                        if (!animationDraw && !sceneAnimation && timer->Enabled)
                                timer->Stop();
                }
        }
        property bool SceneChanged;

        virtual void CreateHandle() override
        {
                System::Windows::Forms::Form::CreateHandle();

                DoubleBuffered = false;

                if (!device)
                        device = D2DDevice::FromHwnd(Handle);

                graphics = gcnew D2DGraphics(device);
                timer->Tick += (ss, ee) =>
                    {
                            if (!SceneAnimation || SceneChanged)
                            {
                                    OnFrame();
                                    Invalidate();
                                    SceneChanged = false;
                            }
                    };
        }

        virtual void OnPaintBackground(System::Windows::Forms::PaintEventArgs^ e) override
        { }

        virtual override void OnPaint(System::Windows::Forms::PaintEventArgs^ e) override
        {
                if (DesignMode)
                {
                        e->Graphics->Clear(System::Drawing::Color::Black);
                        e->Graphics->DrawString("D2DLib windows form cannot render in design time.",
                            Font, System::Drawing::Brushes::White, 10, 10);
                }
                else
                {
                        if (backgroundImage)
                        {
                                graphics->BeginRender(backgroundImage);
                        }
                        else
                        {
                                graphics->BeginRender(D2DColor::FromGDIColor(BackColor));
                        }

                        OnRender(graphics);

                        if (ShowFPS)
                        {
                                if (lastFpsUpdate.Second != DateTime::Now.Second)
                                {
                                        lastFps = currentFps;
                                        currentFps = 0;
                                        lastFpsUpdate = DateTime.Now;
                                }
                                else
                                {
                                        currentFps++;
                                }

                                String^ fpsInfo = String::Format("{0} fps", lastFps);
                                System::Drawing::SizeF size = e->Graphics->MeasureString(fpsInfo, Font, Width);
                                graphics->DrawText(fpsInfo, D2DColor::Silver, Font,
                                    System::Drawing::PointF(ClientRectangle.Right - size.Width - 10, 5));
                        }

                        graphics->EndRender();

                        if ((animationDraw || sceneAnimation) && !timer->Enabled)
                        {
                                timer->Start();
                        }
                }
        }

        virtual void WndProc(System::Windows::Forms::Message% m) override
        {
                switch (m->Msg)
                {
                case (int)WindowsMessages::WM_ERASEBKGND:
                        break;

                case (int)WindowsMessages::WM_SIZE:
                        System::Windows::Forms::Form::WndProc(m);
                        if (this.device != null)
                        {
                                device->Resize();
                                Invalidate(false);
                        }
                        break;

                case (int)WindowsMessages::WM_DESTROY:
                        if (backgroundImage)
                                backgroundImage->Dispose();
                        if (device)
                                device->Dispose();
                        System::Windows::Forms::Form::WndProc(m);
                        break;

                default:
                        System::Windows::Forms::Form::WndProc(m);
                        break;
                }
        }

        virtual void OnRender(D2DGraphics^ g) { }

        virtual void OnFrame() { }

	virtual void OnKeyDown(KeyEventArgs^ e) override
        {
                System::Windows::Forms::Form::OnKeyDown(e);

                switch (e->KeyCode)
                {
                case Keys::Escape:
                        if (EscapeKeyToClose)
                                Close();
                        break;
                }
        }
private:
        D2DDevice^ device;
        D2DBitmap^ backgroundImage;
        D2DGraphics^ graphics;
        int currentFps;
        int lastFps;
        DateTime lastFpsUpdate = DateTime.Now;
        Timer timer;
        bool animationDraw;
        bool sceneAnimation;



};

public ref struct D2DControl : System::Windows::Forms::Control
{
        property D2DDevice^ Device
        {
                D2DDevice^ get()
                {
                        auto hwnd = Handle;
                        if (!device)
                                device = D2DDevice::FromHwnd(hwnd);
                        return device;
                }
        }

        property bool ShowFPS;

        new void Invalidate()
        {
                System::Windows::Forms::Control::Invalidate(false);
        }


        property new D2DBitmap^ BackgroundImage
	{
                D2DBitmap^ get() { return backgroundImage; }
                void set(D2DBitmap^ value)
                {
                        if (backgroundImage != value)
                        {
                                if (backgroundImage)
                                        backgroundImage->Dispose();
                                backgroundImage = value;
                                Invalidate();

                        }
                }
        }

        virtual void CreateHandle() override
        {
                System::Windows::Forms::ControlCreateHandle();

                DoubleBuffered = false;

                if (!device)
                        device = D2DDevice::FromHwnd(Handle);

                graphics = gcnew D2DGraphics(device);
        }


protected:
        virtual void OnPaintBackground(System::Windows::Forms::PaintEventArgs^ e) override { }

        virtual void OnPaint(System::Windows::Forms::PaintEventArgs^ e) override
        {
                if (backgroundImage)
				graphics->BeginRender(backgroundImage);
			else
				graphics->BeginRender(D2DColor::FromGDIColor(BackColor));

                OnRender(graphics);

                if (ShowFPS)
                {
                        if (lastFpsUpdate.Second != DateTime.Now.Second)
                        {
                                lastFps = currentFps;
                                currentFps = 0;
                        }
                        else
                                currentFps++;

                        auto info = String::Format("{0} fps", lastFps);
                        auto size = e->Graphics->MeasureString(info, Font, Width);
                        e->Graphics->DrawString(info, Font,
                            System::Drawing::Brushes::Black,
                            ClientRectangle.Right - size.Width - 10, 5);
                }

                graphics->EndRender();
        }

        virtual void DestroyHandle() override
        {
                System::Windows::Forms::Control::DestroyHandle();
                device->Dispose();
        }

		virtual void OnRender(D2DGraphics g) override { }

        virtual void WndProc(System::Windows::Forms::Message% m) override
        {
                switch (m->Msg)
                {
                case (int)WindowsMessages::WM_ERASEBKGND:
                        break;

                case (int)WindowsMessages::WM_SIZE:
                        System::Windows::Forms::Control::WndProc(m);
                        if (device)
                                device->Resize();
                        break;

                default:
                        System::Windows::Forms::Control::WndProc(m);
                        break;
                }
        }

private:
        D2DDevice^ device;
        DateTime lastFpsUpdate = DateTime.Now;
        D2DGraphics^ graphics;
        int currentFps;
        int lastFps;
        D2DBitmap^ backgroundImage;

}

}

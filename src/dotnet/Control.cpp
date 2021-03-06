namespace Direct2D {


public ref struct D2DForm : System::Windows::Forms::Form
{
        D2DForm() : System::Windows::Forms::Form()
        {
                timer = gcnew System::Windows::Forms::Timer();
                timer->Interval = 10;
        }

        property D2DDevice^ Device
        {
                D2DDevice^ get()
                {
                        auto hwnd = Handle;
                        if (!device)
                                device = D2DDevice::FromHwnd(hwnd.ToPointer());
                        return device;
		}
        }

        property D2DBitmap^ BackgroundImage
        {
                virtual D2DBitmap^ get() new { return backgroundImage; }
                virtual void set(D2DBitmap^ value)
                {
                        if (backgroundImage != value)
                        {
                                if (backgroundImage)
                                        backgroundImage->~D2DBitmap();
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

        virtual void Invalidate() new
        {
                System::Windows::Forms::Form::Invalidate(false);
        }

protected:
        property bool SceneAnimation
        {
                bool get() { return sceneAnimation; }
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
                        device = D2DDevice::FromHwnd(Handle.ToPointer());

                graphics = gcnew D2DGraphics(device);
                timer->Tick += gcnew EventHandler(this, &D2DForm::OnTick);
        }

        void OnTick(Object^ sender, EventArgs^ args)
        {
                if (!SceneAnimation || SceneChanged)
                {
                        OnFrame();
                        Invalidate();
                        SceneChanged = false;
                }
        }

        virtual void OnPaintBackground(System::Windows::Forms::PaintEventArgs^ e) override
        { }

        virtual void OnPaint(System::Windows::Forms::PaintEventArgs^ e) override
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
                                        lastFpsUpdate = DateTime::Now;
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
                switch (m.Msg)
                {
                case (int)WindowsMessages::WMERASEBKGND:
                        break;

                case (int)WindowsMessages::WMSIZE:
                        System::Windows::Forms::Form::WndProc(m);
                        if (device)
                        {
                                device->Resize();
                                Invalidate(false);
                        }
                        break;

                case (int)WindowsMessages::WMDESTROY:
                        if (backgroundImage)
                                backgroundImage->~D2DBitmap();
                        if (device)
                                device->~D2DDevice();
                        System::Windows::Forms::Form::WndProc(m);
                        break;

                default:
                        System::Windows::Forms::Form::WndProc(m);
                        break;
                }
        }

        virtual void OnRender(D2DGraphics^ g) { }

        virtual void OnFrame() { }

	virtual void OnKeyDown(System::Windows::Forms::KeyEventArgs^ e) override
        {
                System::Windows::Forms::Form::OnKeyDown(e);

                switch (e->KeyCode)
                {
                case System::Windows::Forms::Keys::Escape:
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
        DateTime lastFpsUpdate = DateTime::Now;
        System::Windows::Forms::Timer^ timer;
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
                                device = D2DDevice::FromHwnd(hwnd.ToPointer());
                        return device;
                }
        }

        property bool ShowFPS;

        virtual void Invalidate() new
        {
                System::Windows::Forms::Control::Invalidate(false);
        }


        property D2DBitmap^ BackgroundImage
	{
                virtual D2DBitmap^ get() new { return backgroundImage; }
                virtual void set(D2DBitmap^ value)
                {
                        if (backgroundImage != value)
                        {
                                if (backgroundImage)
                                        backgroundImage->~D2DBitmap();
                                backgroundImage = value;
                                Invalidate();

                        }
                }
        }

        virtual void CreateHandle() override
        {
                System::Windows::Forms::Control::CreateHandle();

                DoubleBuffered = false;

                if (!device)
                        device = D2DDevice::FromHwnd(Handle.ToPointer());

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
                        if (lastFpsUpdate.Second != DateTime::Now.Second)
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
                device->~D2DDevice();
        }

        virtual void OnRender(D2DGraphics^ g) { }

        virtual void WndProc(System::Windows::Forms::Message% m) override
        {
                switch (m.Msg)
                {
                case (int)WindowsMessages::WMERASEBKGND:
                        break;

                case (int)WindowsMessages::WMSIZE:
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
        DateTime lastFpsUpdate = DateTime::Now;
        D2DGraphics^ graphics;
        int currentFps;
        int lastFps;
        D2DBitmap^ backgroundImage;

};

}

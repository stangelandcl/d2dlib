namespace Direct2D {

public ref struct D2DForm : System::Windows::Forms::Form
{
        D2DForm() : System::Windows::Forms::Form() {
                timer = Timer { Interval = 10 };
        }

		public D2DDevice Device
		{
			get
			{
				var hwnd = this.Handle;
				if (this.device == null)
				{
					this.device = D2DDevice.FromHwnd(hwnd);
				}
				return this.device;
			}
		}

		public new D2DBitmap BackgroundImage
		{
			get { return this.backgroundImage; }
			set
			{
				if (this.backgroundImage != value)
				{
					if (this.backgroundImage != null)
					{
						this.backgroundImage.Dispose();
					}
					this.backgroundImage = value;
					Invalidate();
				}
			}
		}


		public bool ShowFPS { get; set; }
		public bool EscapeKeyToClose { get; set; } = true;

		public bool AnimationDraw
		{
			get { return this.animationDraw; }
			set
			{
				this.animationDraw = value;
				if (!this.animationDraw && !this.sceneAnimation && timer.Enabled) timer.Stop();
			}
		}


		protected bool SceneAnimation
		{
			get { return this.sceneAnimation; }
			set
			{
				this.sceneAnimation = true;
				if (!this.animationDraw && !this.sceneAnimation && timer.Enabled) timer.Stop();
			}
		}
		protected bool SceneChanged { get; set; }

		protected override void CreateHandle()
		{
			base.CreateHandle();

			this.DoubleBuffered = false;

			if (this.device == null)
			{
				this.device = D2DDevice.FromHwnd(this.Handle);
			}

			this.graphics = new D2DGraphics(this.device);
			this.timer.Tick += (ss, ee) =>
			{
				if (!SceneAnimation || SceneChanged)
				{
					OnFrame();
					Invalidate(); SceneChanged = false;
				}
			};
		}

		protected override void OnPaintBackground(System.Windows.Forms.PaintEventArgs e) { }

		protected override void OnPaint(System.Windows.Forms.PaintEventArgs e)
		{
			if (this.DesignMode)
			{
				e.Graphics.Clear(System.Drawing.Color.Black);
				e.Graphics.DrawString("D2DLib windows form cannot render in design time.", this.Font, System.Drawing.Brushes.White, 10, 10);
			}
			else
			{
				if (this.backgroundImage != null)
				{
					this.graphics.BeginRender(this.backgroundImage);
				}
				else
				{
					this.graphics.BeginRender(D2DColor.FromGDIColor(this.BackColor));
				}

				OnRender(this.graphics);

				if (ShowFPS)
				{
					if (this.lastFpsUpdate.Second != DateTime.Now.Second)
					{
						this.lastFps = this.currentFps;
						this.currentFps = 0;
						this.lastFpsUpdate = DateTime.Now;
					}
					else
					{
						this.currentFps++;
					}

					string fpsInfo = string.Format("{0} fps", lastFps);
					System.Drawing.SizeF size = e.Graphics.MeasureString(fpsInfo, Font, Width);
					this.graphics.DrawText(fpsInfo, unvell.D2DLib.D2DColor.Silver, Font,
						new System.Drawing.PointF(ClientRectangle.Right - size.Width - 10, 5));
				}

				this.graphics.EndRender();

				if ((this.animationDraw || this.sceneAnimation) && !this.timer.Enabled)
				{
					this.timer.Start();
				}
			}
		}

		protected override void WndProc(ref System.Windows.Forms.Message m)
		{
			switch (m.Msg)
			{
				case (int)unvell.Common.Win32Lib.Win32.WMessages.WM_ERASEBKGND:
					break;

				case (int)unvell.Common.Win32Lib.Win32.WMessages.WM_SIZE:
					base.WndProc(ref m);
					if (this.device != null)
					{
						this.device.Resize();
						Invalidate(false);
					}
					break;

				case (int)unvell.Common.Win32Lib.Win32.WMessages.WM_DESTROY:
					if (this.backgroundImage != null) this.backgroundImage.Dispose();
					if (this.device != null) this.device.Dispose();
					base.WndProc(ref m);
					break;

				default:
					base.WndProc(ref m);
					break;
			}
		}

		protected virtual void OnRender(D2DGraphics g) { }

		protected virtual void OnFrame() { }

		public new void Invalidate()
		{
			base.Invalidate(false);
		}

		protected override void OnKeyDown(KeyEventArgs e)
		{
			base.OnKeyDown(e);

			switch (e.KeyCode)
			{
				case Keys.Escape:
					if (EscapeKeyToClose) Close();
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

	public class D2DControl : System.Windows.Forms.Control
	{
		private D2DDevice device;

		public D2DDevice Device
		{
			get
			{
				var hwnd = this.Handle;
				if (this.device == null)
				{
					this.device = D2DDevice.FromHwnd(hwnd);
				}
				return this.device;
			}
		}

		private D2DGraphics graphics;

		private int currentFps = 0;
		private int lastFps = 0;
		public bool ShowFPS { get; set; }
		private DateTime lastFpsUpdate = DateTime.Now;

		protected override void CreateHandle()
		{
			base.CreateHandle();

			this.DoubleBuffered = false;

			if (this.device == null)
			{
				this.device = D2DDevice.FromHwnd(this.Handle);
			}

			this.graphics = new D2DGraphics(this.device);
		}

		private D2DBitmap backgroundImage = null;

		public new D2DBitmap BackgroundImage
		{
			get { return this.backgroundImage; }
			set
			{
				if (this.backgroundImage != value)
				{
					if (this.backgroundImage != null)
					{
						this.backgroundImage.Dispose();
					}
					this.backgroundImage = value;
					Invalidate();
				}
			}
		}

		protected override void OnPaintBackground(System.Windows.Forms.PaintEventArgs e) { }

		protected override void OnPaint(System.Windows.Forms.PaintEventArgs e)
		{
			if (this.backgroundImage != null)
			{
				this.graphics.BeginRender(this.backgroundImage);
			}
			else
			{
				this.graphics.BeginRender(D2DColor.FromGDIColor(this.BackColor));
			}

			OnRender(this.graphics);

			if (ShowFPS)
			{
				if (this.lastFpsUpdate.Second != DateTime.Now.Second)
				{
					this.lastFps = this.currentFps;
					this.currentFps = 0;
				}
				else
				{
					this.currentFps++;
				}

				string info = string.Format("{0} fps", this.lastFps);
				System.Drawing.SizeF size = e.Graphics.MeasureString(info, Font, Width);
				e.Graphics.DrawString(info, Font, System.Drawing.Brushes.Black, ClientRectangle.Right - size.Width - 10, 5);
			}

			this.graphics.EndRender();
		}

		protected override void DestroyHandle()
		{
			base.DestroyHandle();
			this.device.Dispose();
		}

		protected virtual void OnRender(D2DGraphics g) { }

		protected override void WndProc(ref System.Windows.Forms.Message m)
		{
			switch (m.Msg)
			{
				//case (int)unvell.Common.Win32Lib.Win32.WMessages.WM_PAINT:
				//	base.WndProc(ref m);
				//	break;

				case (int)unvell.Common.Win32Lib.Win32.WMessages.WM_ERASEBKGND:
					break;

				case (int)unvell.Common.Win32Lib.Win32.WMessages.WM_SIZE:
					base.WndProc(ref m);
					if (this.device != null) this.device.Resize();
					break;

				default:
					base.WndProc(ref m);
					break;
			}
		}

		public new void Invalidate()
		{
			base.Invalidate(false);
		}
	}

}

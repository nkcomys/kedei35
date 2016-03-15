using System;
using System.Runtime.InteropServices;



public class Program {

	public static void Main() {
		using(var display = new LcdDisplay()) {
			display.Init();
			display.Clear(0x00);
			display.Rectangle(10,10,470,310,0xff);
			display.EmptyRectangle(50,50,200,300,0xfa,0xaf);
		}
	}
}


public class LcdDisplay : IDisposable
{
    [DllImport("libkedei.so")]	
    private static extern void lcd_open();
    [DllImport("libkedei.so")]
    private static extern void lcd_init();
    [DllImport("libkedei.so")]
    private static extern void lcd_close();
    [DllImport("libkedei.so")]
    private static extern void lcd_clear(UInt32 clr);
	[DllImport("libkedei.so")]
    private static extern void lcd_rectangle(UInt32 x, UInt32 y, UInt32 ex, UInt32 ey, UInt32 clr);
	[DllImport("libkedei.so")]
    private static extern void lcd_rectangle_empty(UInt32 x, UInt32 y, UInt32 ex, UInt32 ey,  UInt32 clr1, UInt32 clr2);

    public void Init() {
    	lcd_open();
    	lcd_init();
    }

    public void Rectangle(UInt32 x, UInt32 y, UInt32 ex, UInt32 ey, UInt32 color)
    {
    	lcd_rectangle(x,y,ex,ey,color);
    }

    public void EmptyRectangle(UInt32 x, UInt32 y, UInt32 ex, UInt32 ey,  UInt32 borderColor, UInt32 background)
    {
    	lcd_rectangle_empty(x,y,ex,ey,borderColor, background);
    }

    public void Clear(UInt32 color) {
    	lcd_clear(color);
    }

    public void Dispose() {
    	lcd_close();
    }
}

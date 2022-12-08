package main

/*
#include "lib/bbb_dht_read.h"
#include "lib/bbb_dht_read.c"
#include "lib/bbb_mmio.c"
#include "lib/common_dht_read.c"
*/
import "C"

import (
	"fmt"
	"image"
	"os"
	"time"

	"github.com/golang/freetype/truetype"

	"gobot.io/x/gobot"
	"gobot.io/x/gobot/drivers/i2c"
	"gobot.io/x/gobot/platforms/beaglebone"
	"golang.org/x/image/font"
	"golang.org/x/image/font/gofont/gosmallcaps"
	"golang.org/x/image/math/fixed"
)

const (
	DHT22 = 22
	P9    = 1
	PIN12 = 28
)

func main() {
	master := gobot.NewMaster()

	width := 128
	height := 64

	beagleboneAdaptor := beaglebone.NewAdaptor()

	oled := i2c.NewSSD1306Driver(beagleboneAdaptor, i2c.WithSSD1306DisplayWidth(width), i2c.WithSSD1306DisplayHeight(height))

	content, err := newFace(20)

	if err != nil {
		fmt.Fprintln(os.Stderr, err)
		return
	}

	work := func() {
		gobot.Every(2*time.Second, func() {

			hum := C.float(0.00)
			temp := C.float(0.00)
			C.bbb_dht_read(DHT22, P9, PIN12, &hum, &temp)

			oled.Clear()
			img := textToImage(fmt.Sprintf("%.2f ºC", temp), fmt.Sprintf("%.2f %%", hum), content, width, height)
			oled.ShowImage(img)
			oled.Display()
		})
	}

	robot := gobot.NewRobot("ssd1306Robot",
		[]gobot.Connection{beagleboneAdaptor},
		[]gobot.Device{oled},
		work,
	)

	master.AddRobot(robot)

	master.Start()
}

func newFace(size float64) (face font.Face, err error) {
	ft, err := truetype.Parse(gosmallcaps.TTF)
	if err != nil {
		return
	}
	opt := truetype.Options{
		Size:              size,
		DPI:               0,
		Hinting:           0,
		GlyphCacheEntries: 0,
		SubPixelsX:        0,
		SubPixelsY:        0,
	}
	face = truetype.NewFace(ft, &opt)
	return
}

func textToImage(text string, text2 string, face font.Face, width int, height int) image.Image {
	img := image.NewRGBA(image.Rect(0, 0, width, height))

	title, err := newFace(10)
	if err != nil {
		fmt.Fprintln(os.Stderr, err)
		return nil
	}

	dr := &font.Drawer{
		Dst:  img,
		Src:  image.White,
		Face: face,
		Dot:  fixed.Point26_6{},
	}
	ds := &font.Drawer{
		Dst:  img,
		Src:  image.White,
		Face: title,
		Dot:  fixed.Point26_6{},
	}

	umidade_string := "Umidade (%)"
	ds.Dot.X = (fixed.I(width) - ds.MeasureString(umidade_string))
	ds.Dot.Y = fixed.I(10)

	ds.DrawString(umidade_string)

	dr.Dot.X = (fixed.I(width) - dr.MeasureString(text2))
	dr.Dot.Y = fixed.I(10 + 20 + 1)
	dr.DrawString(text2)

	temperatura_string := "Temperatura (ºC)"
	ds.Dot.X = (fixed.I(width) - ds.MeasureString(umidade_string))
	ds.Dot.Y = fixed.I(10 + 20 + 1 + 10)
	ds.DrawString(temperatura_string)

	dr.Dot.X = (fixed.I(width) - dr.MeasureString(text))
	dr.Dot.Y = fixed.I(10 + 20 + 1 + 10 + 20)
	dr.DrawString(text)

	return img
}

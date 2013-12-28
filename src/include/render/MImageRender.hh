// File: MImageRender.hh
// Date: Sun Dec 22 14:35:25 2013 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#pragma once
#include "render/render.hh"
#include <Magick++.h>

class MImg : public RenderBase {
	private:
		Magick::Image img;
		Magick::Pixels *view;
		Magick::PixelPacket *pixels_ptr;

	public:
		MImg(const Geometry& g):
			RenderBase(g), img(Magick::Geometry(g.w, g.h), "black")
		{
			img.type(Magick::TrueColorType);
			img.modifyImage();
			view = new Magick::Pixels(img);
			pixels_ptr = view->get(0, 0, g.w, g.h);
		}

		MImg(int w, int h):
			MImg(Geometry(w, h)){}

		explicit MImg(const std::shared_ptr<Img>& r):
			MImg(r->w, r->h)
		{ write(r); }

		explicit MImg(const std::shared_ptr<GreyImg>& r):
			MImg(r->w, r->h)
		{ write(r->to_img()); }

		~MImg() { delete view; }

		Magick::Image get_img() const
		{ return img; }


	private:
		void _write(int x, int y, const Color &c)
		{ pixels_ptr[y * geo.w + x] = Magick::ColorRGB(c.x, c.y, c.z); }

};


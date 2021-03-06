// -*-c++-*- Copyright (C) 2010 osgPango Development Team
// $Id$

#include <sstream>
#include <osg/Texture2D>
#include <osg/MatrixTransform>
#include <osgGA/StateSetManipulator>
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgPairo/TextTransform>

const std::string LOREM_IPSUM(
	"Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod "
	"tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, "
	"quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. "
	"Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu "
	"fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in "
	"culpa qui officia deserunt mollit anim id est laborum."
);

struct GlyphLayerGradient: public osgPairo::GlyphLayer {
	virtual bool render(
		cairo_t*       c,
		cairo_glyph_t* glyph,
		unsigned int   width,
		unsigned int   height
	) {
		if(cairo_status(c) || !glyph) return false;

		cairo_set_line_width(c, 1.5f);
		cairo_glyph_path(c, glyph, 1);

		cairo_pattern_t* lp = cairo_pattern_create_linear(width / 2.0f, 0.0f, width / 2.0f, height);

		cairo_pattern_add_color_stop_rgba(lp, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
		cairo_pattern_add_color_stop_rgba(lp, 0.8f, 0.0f, 1.0f, 0.0f, 0.0f);
		cairo_set_source(c, lp);
		cairo_fill(c);
		cairo_pattern_destroy(lp);

		return true;
	}
};

struct GlyphRendererGradient: public osgPairo::GlyphRendererDefault {
	GlyphRendererGradient() {
		replaceLayer(0, new GlyphLayerGradient());
	}
};

osg::Camera* createOrthoCamera(float width, float height) {
	osg::Camera* camera = new osg::Camera();

	camera->getOrCreateStateSet()->setMode(
		GL_LIGHTING,
		osg::StateAttribute::PROTECTED | osg::StateAttribute::OFF
	);

	camera->setProjectionMatrix(osg::Matrix::ortho2D(0.0, width, 0.0f, height));
	camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	camera->setViewMatrix(osg::Matrix::identity());
	camera->setClearMask(GL_DEPTH_BUFFER_BIT);
	camera->setRenderOrder(osg::Camera::POST_RENDER);

	return camera;
}

int main(int argc, char** argv) {
	osgPairo::Context& context = osgPairo::Context::instance();

	context.init();
	context.addGlyphRenderer("gradient", new GlyphRendererGradient());

	osgPairo::TextTransform* t = new osgPairo::TextTransform();

	std::ostringstream os;

	os << "<span font='Verdana Bold 40'>" << LOREM_IPSUM << "</span>";

	t->setGlyphRenderer("gradient");
	t->setText(os.str().c_str(), osgPairo::TextOptions(
		osgPairo::TextOptions::TEXT_ALIGN_CENTER,
		750
	));

	t->finalize();
	// t->setPosition(t->getOriginTranslated());

	osgViewer::Viewer viewer;

	osg::Group*  group  = new osg::Group();
	osg::Camera* camera = createOrthoCamera(800, 600);
	osg::Node*   node   = osgDB::readNodeFile("cow.osg");

        viewer.addEventHandler(new osgViewer::StatsHandler());
        viewer.addEventHandler(new osgViewer::WindowSizeHandler());
        viewer.addEventHandler(new osgGA::StateSetManipulator(
                viewer.getCamera()->getOrCreateStateSet()
        ));

	camera->addChild(t);

	group->addChild(node);
	group->addChild(camera);

	viewer.setSceneData(group);
	viewer.getCamera()->setClearColor(osg::Vec4(0.3f, 0.3f, 0.3f, 1.0f));
	viewer.setUpViewInWindow(50, 50, 800, 600);

	viewer.run();

	// osgPairo::Context::instance().writeCachesToPNGFiles("osgpangocustomrenderer");

	return 0;
}

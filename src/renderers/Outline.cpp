// -*-c++-*- Copyright (C) 2011 osgPango Development Team
// $Id$

#include <osgPairo/GlyphRenderer>

namespace osgPairo {

GlyphRendererOutline::GlyphRendererOutline(unsigned int outline) {
	addLayer(new GlyphLayer());
	addLayer(new GlyphLayerOutline(outline));
}

GlyphRendererOutline::GlyphRendererOutline(
	const GlyphRendererOutline& gro,
	const osg::CopyOp&          copyOp
):
GlyphRenderer(gro, copyOp) {
}

bool GlyphRendererOutline::updateOrCreateState(int pass, osg::Geode* geode) const {
	if(!GlyphRenderer::updateOrCreateState(pass, geode)) return false;

	return _setFragmentShader(geode, "osgPango-frag2");
}

}

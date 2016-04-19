// -*-c++-*- Copyright (C) 2011 osgPango Development Team
// $Id$

#include <osgDB/Registry>
#include <osgDB/ObjectWrapper>
#include <osgDB/InputStream>
#include <osgDB/OutputStream>
#include <osgCairo/Util>
#include <osgPango3/Glyph>

// ----------------------------------------------------------------------------------------- Layers
static bool checkLayers(const osgPango3::GlyphCache& gc) {
	const osgPango3::GlyphCache::Layers& layers = gc.getLayers();

	return layers.size() != 0;
}

static bool readLayers(osgDB::InputStream& is, osgPango3::GlyphCache& gc) {
	osgPango3::GlyphCache::Layers& layers = gc.getLayers();

	unsigned int layerSize = is.readSize();
	unsigned int imgSize   = is.readSize();

	is >> is.BEGIN_BRACKET;

	for(unsigned int l = 0; l < layerSize; l++) {
		layers.push_back(osgPango3::GlyphCache::Images());

		is >> is.PROPERTY("Layer") >> is.BEGIN_BRACKET;

		for(unsigned int i = 0; i < imgSize; i++) {
			osgPango3::GlyphCache::CairoTexture ct;

                        osg::ref_ptr<osg::Object> obj = is.readObject();
			osg::Texture* texture = dynamic_cast<osg::Texture*>(obj.get());

			if(!texture) {
				// TODO: More verbose...
				OSG_WARN << "Couldn't load Image " << i << std::endl;

				return false;
			}

			osg::Image* image = texture->getImage(0);

			if(!image) {
				OSG_WARN << "No image..." << std::endl;

				return false;
			}

			osgCairo::Image* newImage = new osgCairo::Image();

			if(newImage->allocateSurface(image)) {
				// *
				cairo_t* c = newImage->createContext();

				cairo_arc(c, image->s() / 2.0f, image->t() / 2.0f, 100.0f, 0.0f, 2.0f * 3.14159f);
				cairo_fill(c);
				cairo_destroy(c);
				// *
			}

			newImage->setFileName(image->getFileName());

			// Now the previous image will be deleted, but our data has
			// been copied so it's all good.
			texture->setImage(0, newImage);

			ct.first  = newImage;
			ct.second = texture;

			layers[l].push_back(ct);
		}

		is >> is.END_BRACKET;
	}

	is >> is.END_BRACKET;

	return true;
}

static bool writeLayers(osgDB::OutputStream& os, const osgPango3::GlyphCache& gc) {
	const osgPango3::GlyphCache::Layers& layers = gc.getLayers();

	os.writeSize(layers.size());
	os.writeSize(layers[0].size());

	os << os.BEGIN_BRACKET << std::endl;

	// We have one Layer per "effect", essentially.
	for(unsigned int l = 0; l < layers.size(); l++) {
		// Now we iterate over our Images, which we can have a variable number of
		// depending on how numerous and how large the glyphs are.

		os << "Layer" << os.BEGIN_BRACKET << std::endl;

		for(unsigned int i = 0; i < layers[l].size(); i++) {
			osgCairo::Image* image   = layers[l][i].first.get();
			osg::Texture*    texture = layers[l][i].second.get();

			// TODO: SPEW ERRORS...
			if(!image->getFileName().size()) {
				OSG_WARN << "No filename!" << std::endl;

				return false;
			}

			// TODO: Not necessary for A8 surfaces...
			// image->unPreMultiply();

			os.setWriteImageHint(osgDB::OutputStream::WRITE_EXTERNAL_FILE);
			os.writeObject(texture);

			// image->premultiply();
		}

		os << os.END_BRACKET << std::endl;
	}

	os << os.END_BRACKET << std::endl;

	return true;
}

// --------------------------------------------------------------------------------------- GlyphMap
static bool checkGlyphMap(const osgPango3::GlyphCache& gc) {
	const osgPango3::GlyphCache::GlyphMap& gmap = gc.getGlyphMap();

	return gmap.size() != 0;
}

static bool readGlyphMap(osgDB::InputStream& is, osgPango3::GlyphCache& gc) {
	osgPango3::GlyphCache::GlyphMap& gmap = gc.getGlyphMap();

	unsigned int numGlyphs = is.readSize();

	is >> is.BEGIN_BRACKET;

	for(unsigned int g = 0; g < numGlyphs; g++) {
		unsigned int glyphID;

		is >> glyphID >> is.BEGIN_BRACKET;

		osgPango3::CachedGlyph cg;

		is >> is.PROPERTY("img") >> cg.img;
		is >> is.PROPERTY("origin") >> cg.origin;
		is >> is.PROPERTY("size") >> cg.size;
		is >> is.PROPERTY("bl") >> cg.bl;
		is >> is.PROPERTY("br") >> cg.br;
		is >> is.PROPERTY("ur") >> cg.ur;
		is >> is.PROPERTY("ul") >> cg.ul;

		is >> is.END_BRACKET;

		gmap[glyphID] = cg;
	}

	is >> is.END_BRACKET;

	return true;
}

static bool writeGlyphMap(osgDB::OutputStream& os, const osgPango3::GlyphCache& gc) {
	const osgPango3::GlyphCache::GlyphMap& gmap = gc.getGlyphMap();

	os.writeSize(gmap.size());

	os << os.BEGIN_BRACKET << std::endl;

	for(
		osgPango3::GlyphCache::GlyphMap::const_iterator i = gmap.begin();
		i != gmap.end();
		i++
	) {
		os << i->first << os.BEGIN_BRACKET << std::endl;

		const osgPango3::CachedGlyph& cg = i->second;

		os << "img" << cg.img << std::endl;
		os << "origin" << cg.origin << std::endl;
		os << "size" << cg.size << std::endl;
		os << "bl" << cg.bl << std::endl;
		os << "br" << cg.br << std::endl;
		os << "ur" << cg.ur << std::endl;
		os << "ul" << cg.ul << std::endl;

		os << os.END_BRACKET << std::endl;
	}

	os << os.END_BRACKET << std::endl;

	return true;
}

REGISTER_OBJECT_WRAPPER(
	osgPango3_GlyphCache,
	new osgPango3::GlyphCache(),
	osgPango3::GlyphCache,
	"osg::Object osgPango3::GlyphCache"
) {
	ADD_UINT_SERIALIZER(Hash, 0);

	ADD_STRING_SERIALIZER(Description, "");

	ADD_VEC3F_SERIALIZER(XYH, osg::Vec3f());

	ADD_USER_SERIALIZER(Layers);
	ADD_USER_SERIALIZER(GlyphMap);
}


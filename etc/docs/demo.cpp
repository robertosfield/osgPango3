// First, get yourself a new Image object:

	osgPairo::Image* image = new osgPairo::Image();

	if(image->allocateImage(128, 128) && image->createContext()) {}

// ...OR...

	osgPairo::Image* image = new osgPairo::Image(
		128,
		128,
		CAIRO_FORMAT_ARGB32,
		pointerToRGBAdata
	);

	if(image->createContext()) {}

// Secondly, call the various Cairo methods on the image:

	image->setSourceRGBA(1.0f, 1.0f, 0.0f);
	image->moveTo(0.0f, 0.0f);
	image->lineTo(10.0f, 10.0f);
	image->stroke();

// Thirdly, once you're finished drawing--since an osgPairo::Image is
// also an osg::Image--inform OSG that the Image has been modified and is
// dirty:

	image->dirty();

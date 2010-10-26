#include "RefMap.h"

using namespace BWAPI;

template <typename Ref>
RefMap<Ref>::RefMap()
  : _w(0),
    _h(0),
		_xBins(0),
		_yBins(0),
		_binWidth(0),
		_binHeight(0),
		_refs()
{
}

template <typename Ref>
RefMap<Ref>::RefMap(int width, int height, int xBins, int yBins)
  : _w(width),
    _h(height),
		_xBins(xBins),
		_yBins(yBins),
		_binWidth(((double) width) / xBins),
		_binHeight(((double) height) / yBins),
		_refs(xBins * yBins)
{
}

template <typename Ref>
RefMap<Ref>::RefMap(int width, int height, int xBins, int yBins, Ref r)
  : _w(width),
    _h(height),
		_xBins(xBins),
		_yBins(yBins),
		_binWidth(((double) width) / xBins),
		_binHeight(((double) height) / yBins),
		_refs(xBins * yBins, r)
{
}

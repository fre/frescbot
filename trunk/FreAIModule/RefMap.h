#pragma once

#include <BWAPI.h>

#include <vector>

#include "Common.h"

// Stores references for bins of 2D coordinates (one per bin).
//
// FIXME: Check coordinates, avoid using double if possible.
//        Make sure bins are approx. the same size.
template <typename Ref>
class RefMap
{
public:
	RefMap(); // Size: 0x0.
	RefMap(int width, int height, int xBins, int yBins);
	// Initialize bins.
	RefMap(int width, int height, int xBins, int yBins, Ref r);

	inline const Ref& operator[](BWAPI::Position p) const
	{
		return _refs[(int) (p.x() / _binWidth) + (int) (p.y() / _binHeight) * _xBins];
	}
	inline Ref& operator[](BWAPI::Position p)
	{
		return _refs[(int) (p.x() / _binWidth) + (int) (p.y() / _binHeight) * _xBins];
	}

	inline bool contains(BWAPI::Position p) { return p.x() >= 0 && p.x() < _w && p.y() >= 0 && p.y() < _h; }

	inline const Ref& bin(int binX, int binY) const { return _refs[binX + binY * _xBins]; }
	inline Ref& bin(int binX, int binY) { return _refs[binX + binY * _xBins]; }

	inline BWAPI::Position binCenter(int binX, int binY)
	{
		return BWAPI::Position((int) ((binX + 0.5) * _binWidth), (int) ((binY + 0.5) * _binHeight));
	}

	// Number of bins.
	inline int xBins() { return _xBins; }
	inline int yBins() { return _yBins; }

	inline int width() { return _w; }
	inline int height() { return _h; }

	// Size of the bins.
	inline double binWidth() { return _binWidth; }
	inline double binHeight() { return _binHeight; }

private:
	int _w;
	int _h;
	int _xBins;
	int _yBins;
	double _binWidth;
	double _binHeight;

	std::vector<Ref> _refs;
};

#include "RefMap.hxx"

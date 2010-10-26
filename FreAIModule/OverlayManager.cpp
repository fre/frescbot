#include "OverlayManager.h"

using namespace BWAPI;

#define DEFAULT_OVERLAY_UPDATE_PERIOD 100
#define DEFAULT_OVERLAY_VALUE 0
#define DEFAULT_OVERLAY_COLOR Colors::Cyan

#define OVERLAY_X 520
#define OVERLAY_Y 320
#define OVERLAY_RADIUS 4
#define OVERLAY_SPACE 2

OverlayManager::OverlayManager()
  : TimeLimiter(timeManager->fromMs(DEFAULT_OVERLAY_UPDATE_PERIOD)),
	  _overlays(OverlayManager::OverlayCount),
		_overlayColors(OverlayManager::OverlayCount),
		_clicked(0)
{
	for (int i = 0; i < OverlayCount; ++i)
	{
		_overlays[i] = DEFAULT_OVERLAY_VALUE;
		_overlayColors[i] = DEFAULT_OVERLAY_COLOR;
	}
	_overlays[6] = 1;
	_overlays[7] = 1;
}

void
OverlayManager::update()
{
	// Detect clicks.
	bool justClicked = 0;
	if (Broodwar->getMouseState(BWAPI::M_LEFT))
	{
		if (!_clicked)
		{
			justClicked = 1;
			_clicked = 1;
		}
	}
	else
		_clicked = 0;

	int overlayWidth = OVERLAY_RADIUS * 2 + OVERLAY_SPACE;

	// Update overlay states.
	if (justClicked)
	{
		int y = Broodwar->getMouseY() - OVERLAY_Y;
		if (y >= -OVERLAY_RADIUS && y <= OVERLAY_RADIUS)
			for (int i = 0; i < OverlayCount; ++i)
			{
				int x = Broodwar->getMouseX() - (OVERLAY_X + i * overlayWidth);
				if (x >= -OVERLAY_RADIUS && x <= OVERLAY_RADIUS)
					_overlays[i] = !_overlays[i];
			}
	}
}

void
OverlayManager::frameUpdate()
{
#ifdef DEBUG_DISPLAY
	// Display overlay controls.
	int overlayWidth = OVERLAY_RADIUS * 2 + OVERLAY_SPACE;
	for (int i = 0; i < OverlayCount; ++i)
	{
		if (_overlays[i])
			Broodwar->drawCircle(CoordinateType::Screen, OVERLAY_X + i * overlayWidth, OVERLAY_Y, OVERLAY_RADIUS, Colors::White, true);
		else
			Broodwar->drawCircle(CoordinateType::Screen, OVERLAY_X + i * overlayWidth, OVERLAY_Y, OVERLAY_RADIUS, Colors::Black, true);
		Broodwar->drawCircle(CoordinateType::Screen, OVERLAY_X + i * overlayWidth, OVERLAY_Y, OVERLAY_RADIUS - 1, _overlayColors[i], true);
	}
#endif
}

#pragma once

#include "DrawDebugHelpers.h"

#define DRAW_SPHERE(Location, Color) if(GetWorld()) DrawDebugSphere(GetWorld(), Location, 30.f, 12, Color, true);

#define DRAW_SPHERE_SINGLE_FRAME(Location, Color) if(GetWorld()) DrawDebugSphere(GetWorld(), Location, 30.f, 12, Color, false, -1.f);

#define DRAW_SPHERE_SET_SIZE_AND_DURATION(Location, Color, Radius, Duration) if(GetWorld()) DrawDebugSphere(GetWorld(), Location, Radius, 12, Color, false, Duration);

#define DRAW_LINE(Location, ForwardVector, Length) if(GetWorld()) DrawDebugLine(GetWorld(), Location, Location + (ForwardVector * Length), FColor::Red, true, -1.f, 0, 1.f);

#define DRAW_LINE_SINGLE_FRAME(Location, ForwardVector, Length) if(GetWorld()) DrawDebugLine(GetWorld(), Location, Location + (ForwardVector * Length), FColor::Red, false, -1.f, 0, 1.f);



#define DRAW_POINT(Location, ForwardVector, Length) if(GetWorld()) DrawDebugPoint(GetWorld(), Location + (ForwardVector * Length), 20.f, FColor::Red, true);

#define DRAW_POINT_SINGLE_FRAME(Location, ForwardVector, Length) if(GetWorld()) DrawDebugPoint(GetWorld(), Location + (ForwardVector * Length), 20.f, FColor::Red, false, -1.f);



#define DRAW_VECTOR(Location, ForwardVector, Length, Color) if(GetWorld()) \
	{ \
		DrawDebugLine(GetWorld(), Location, Location + (ForwardVector * Length), Color, true, -1.f, 0, 1.f); \
		DrawDebugPoint(GetWorld(), Location + (ForwardVector * Length), 20.f, Color, true); \
	}

#define DRAW_VECTOR_SINGLE_FRAME(Location, ForwardVector, Length, Color) if(GetWorld()) \
	{ \
		DrawDebugLine(GetWorld(), Location, Location + (ForwardVector * Length), Color, false, -1.f, 0, 1.f); \
		DrawDebugPoint(GetWorld(), Location + (ForwardVector * Length), 20.f, Color, false, -1.f); \
	}



#define DRAW_CROSSHAIR(Location, Rotation, Color) if(GetWorld()) DrawDebugCrosshairs(GetWorld(), Location, Rotation, 10.0f, Color, true);

#define DRAW_CROSSHAIR_SINGLE_FRAME(Location, Rotation, Color) if(GetWorld()) DrawDebugCrosshairs(GetWorld(), Location, Rotation, 10.0f, Color, false, -1.f);
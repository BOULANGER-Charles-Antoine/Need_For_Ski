#pragma once

enum class Layer
{
	Terrain,
	Player,
	Trigger,
	LayerA,
	LayerB,
	LayerC
};

static constexpr const bool LayerCollisions[6][6]{
	/* Terrain,    Player,    Trigger,    LayerA,    LayerB,    LayerC */
	{	false,		 true,		false,		false,	  false,	 false},		//Terrain
	{	true,		 true,		true,		false,	  false,	 false},		//Player
	{	false,		 true,		false,		false,	  false,	 false},		//Trigger
	{	false,		 false,		false,		true,	  false,	 false},		//LayerA
	{	false,		 false,		false,		false,	  true,		 false},		//LayerB
	{	false,		 false,		false,		false,	  false,	 true},			//LayerC
};


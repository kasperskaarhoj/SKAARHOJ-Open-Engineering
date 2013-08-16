/***************************************************************
 *  (c) 2013 SKAARHOJ Engineering, www.skaarhoj.com
 *  Author: Kasper Skårhøj <kasper@skaarhoj.com>
 *
 *  This script is Open Source under GNU/GPL license. 
 *  See bottom for more details.
 *
 *  Licensed to Schaeffer AG for inclusion with Front Panel Designer.
 ***************************************************************/
/*************************************************************
 * This is a JavaScript program written for a software called 
 * Front Panel Designer made by Schaeffer AG. 
 * Front Panel Designer is a CAD application for design and 
 * online ordering of aluminum panels and this script will create 
 * several such panels based on input parameters. The panels will 
 * form a full enclosure when assembled.
 *
 * The enclosures are based on the Housing Manual by Schaeffer AG.
 * Some features are added and modified by SKAARHOJ Engineering.
 *
 * Please watch the demo videos on YouTube published by SKAARHOJ.
 *
 * - kasper
 **************************************************************/


		// TODO:
	// TODO: if lowerRightOffset: DXF contour
	// TODO: Implement tolerances for widths and heights? So far: No.
	// TODO: Optimize standard settings for costs (mostly sizes of the various cavities)
	// Rotation of panels - missing API support for rotating the panel itself (changing its dimensions) and also for returning the correct width!




/***************************************
 * Global Settings for enclosure creation.
 * Adapt to your needs for full flexibility
 * (Alternatively, use the preset functions below)
 *
 * All measures in mm. (25.4 mm = 1 inch)
 ***************************************/

// ENCLOSURE TYPE
// 1 = based on side sections (results in 4 panels)
// 2 = based on housing profiles (results in 6 panels)
var C_type = 2;

// For C_type==1: Side section 1 (42mm high) or 2 (56mm high)?
var C_sideSectionType = 1;

// For C_type==2: Determines if housing profile 2 is used on a given side.
// Notice: If set true for a given side, any adjacent sides CANNOT be true as well!
// For instance, it can be true for Top and Bottom or Left and Right, but not Top and Left
var C_housingProfile2Top = false;
var C_housingProfile2Left = false;
var C_housingProfile2Right = false;
var C_housingProfile2Bottom = false;

// WIDTH, HEIGHT, DEPTH of enclosure (outer measures):
// For C_type==1: Height will be overruled by the height of the chosen side section type 1 (42mm) or 2 (56mm).
var C_width = 300;
var C_height = 100;	
var C_depth = 200;

// Thickness of plates: 2, 2.5, 3, 4
var C_thickness = new Array(
		4, 		// Front
		2.5, 	// Top
		2.5, 	// Back
		2.5, 	// Bottom
		2.5, 	// Left (For C_type==2 only)
		2.5		// Right (For C_type==2 only)
	);

// Colors: These are constants from Schaeffers Scripting API manual
var C_color = new Array(
		elox_blue, 		// Front
		elox_blue, 		// Top
		elox_natural, 	// Back
		elox_blue, 		// Bottom
		elox_blue, 		// Left (For C_type==2 only)
		elox_blue		// Right (For C_type==2 only)
	);

// Housing Brackets:   	
var C_numberOfHousingBracketsFront = new Array(0,0,0,0); // (Top, Bottom, Left, Right). Defines the number of housing brackets used on the front for each side panel. -1 will automatically place them pr. 100 mm.
var C_numberOfHousingBracketsBack = new Array(0,0,0,0);  // (Top, Bottom, Left, Right). Defines the number of housing brackets used on the back for each side panel.  -1 will automatically place them pr. 100 mm.
var C_useStudsForHousingBracketsOnFront = true;	// Uses M3x8mm Studs for housing brackets on front (instead of countersunk holes). Notice: This requires you to drill the thread out of the housing brackets on one side and use a washer and nut to fasten it!
var C_useStudsForHousingBracketsOnBack = false;	// Same as above, but for the back.

// Front and Back Assembly Grooves:
var C_useAssemblyGroovesFront = new Array(true, true, true, true);	// (Top, Bottom, Left, Right). If true, cavities are used on the front panel to fit the top, bottom, left and right plates.
var C_useAssemblyGroovesBack = new Array(true, true, true, true);	// (Top, Bottom, Left, Right). If true, cavities are used on the back panel to fit the top, bottom, left and right plates.
var C_depthOfCavitiesForGroovesFront = new Array(1,1,1,1);	// (Top, Bottom, Left, Right). Depth of cavity in top/bottom/left/right plates, front side
var C_depthOfCavitiesForGroovesBack = new Array(1,1,1,1);	// (Top, Bottom, Left, Right). Depth of cavity in top/bottom/left/right plates, back side
var C_assemblyGrooveDepthFront = new Array(1.5,1.5,1.5,1.5);	// (Top, Bottom, Left, Right). Assembly Groove Depths, front side
var C_assemblyGrooveDepthBack = new Array(1,1,1,1);				// (Top, Bottom, Left, Right). Assembly Groove Depths, back side

// Ordering
var C_createOrder = false;			// If true, will create an order and add the accessories (profiles, assembly kits etc.)
var C_orderBlackItems = false;		// If true, will order accessories in black instead of natural color.

// Additional features:
var C_extraFrontWidth = 0;	// If greater than zero, it will add this amount of space on left and right side of the front panel. This is useful to create "rack ears" (see also "C_rackEars" setting). If you want to create a true 19" unit, make sure 2* this value + C_width equals 483mm. A good choice is to set this value at 23mm and C_width = 437 => 23*2+437=483mm
var C_extraFrontHeight = 0;	// If creating a 1U rack unit, 43.60mm is a typical choice for the total height, so add 1.6 in total by setting this value to 0.8. (1U = 44.45mm or 1.75inch)
var C_rackEars = false;			// If true, adds holes for rack mounting. See also "C_extraFrontWidth"
var C_roundFrontCorners = true;	// If true, corners of the front panel are rounded by 2mm which fits the side or housing profiles.
var C_addEdgeGrinding = true;	// You probably want edges grinded on the end profiles (front and back) if you are not making a rack unit! Otherwise they are ugly from tool marks and often very sharp.
var C_addGuideEngravings = true;	// This will add thin, orange engraved lines for preview of where adjacent components (panels, profiles and brackets) are located on each side. Can be useful for positioning other elements after which they should be removed before ordering.

var C_panelNamesPrefix = "";		// A panel name prefix
var C_panelNamesSuffix = "";		// A panel name suffix

// Exotic features:
var C_sideExpansionOffset = new Array(0,0,0,0); // (Top, Bottom, Left, Right). This offsets the top, bottom, left and right plates outwards if positive. By default the top and bottom plates are placed so their outer surface is flush with the side/housing profiles. When used with housing profile 1 this value should probably be -1!
var C_housingProfileRightHeight = 0;	// Must be less than C_height. If zero = off. Imposes various restrictions on the bottom plate, among other that it has to be bent a bit. So this is a special case. Also, cannot be used with housing brackets on the bottom plate (front and back). 



/***************************************
 * Presets
 *
 * These functions will set groups of the global
 * settings based on a more simple input.
 *
 * Uncomment the one you want to use and 
 * configure the argument values
 ***************************************/

// preset_sideProfileBox(1, 200, 100, elox_blue);	// profile, width, depth, color
// preset_1UnitEnclosure(134, elox_blue);
// preset_housingProfileBox(100, 50, 80, elox_blue);
// preset_rackUnitBox(3, 70, elox_black);
preset_console(108, 60, 40, 135+4+4, elox_blue);
// preset_test();



// ###MARKER_POSTCONFIG###


/**********************************************************************
 * NO USER SETTING BELOW!
 **********************************************************************/


/***************************************
 * Presets functions
 ***************************************/
function preset_sideProfileBox(profile, width, depth, color)	{
	C_type = 1;
	C_sideSectionType = profile==1 ? 1 : 2;

	C_width = width;
	C_depth = depth;
	
	C_color = new Array(
			color, 		// Front
			color, 		// Top
			elox_natural, 	// Back
			color, 		// Bottom
			color,
			color
		);

}
function preset_1UnitEnclosure(depth, color) {
	C_type = 1;
	C_sideSectionType = 1;

	C_depth = depth;
	C_width = 437;

	C_color = new Array(
			color, 		// Front
			color, 		// Top
			elox_natural, 	// Back
			color, 		// Bottom
			color,
			color
		);
		
	C_extraFrontHeight = 0.8;
	C_extraFrontWidth = 23;
	C_rackEars = true;
	C_roundFrontCorners = false;
	
	C_sideExpansionOffset = new Array(0.25,0.25,0,0);	
}
function preset_housingProfileBox(width, height, depth, color) {
	C_type = 2;
	
	C_width = width;
	C_height = height;
	C_depth = depth;
	
	C_color = new Array(
			color, 		// Front
			color, 		// Top
			elox_natural, 	// Back
			color, 		// Bottom
			color, 		// Left (For C_type==2 only)
			color		// Right (For C_type==2 only)
		);
	
	C_housingProfile2Top = true;
	C_sideExpansionOffset = new Array(0,-1,-1,-1);
}
function preset_rackUnitBox(U_height, depth, color) {
	C_type = 2;
	
	C_width = 437;
	C_height = U_height*1.75*25.4-0.5;
	C_depth = depth;
	
	C_extraFrontWidth = 23;
	C_rackEars = true;
	C_roundFrontCorners = false;
		
	C_color = new Array(
			color, 		// Front
			color, 		// Top
			elox_natural, 	// Back
			color, 		// Bottom
			color, 		// Left (For C_type==2 only)
			color		// Right (For C_type==2 only)
		);
	
	C_housingProfile2Top = true;
	
	C_numberOfHousingBracketsFront = new Array(3,3,0,0);
	C_numberOfHousingBracketsBack = new Array(3,3,0,0);
	C_useAssemblyGroovesFront = new Array(true, true, false, false);	// (Top, Bottom, Left, Right).
	C_useAssemblyGroovesBack = new Array(false, false, false, false);	// (Top, Bottom, Left, Right).
}
function preset_console(width, height, height_front, depth, color) {
	C_type = 2;
	
	C_width = width;
	C_height = height;
	C_depth = depth;
	
	C_color = new Array(
			elox_natural, 		// Front
			color, 		// Top
			elox_natural, 	// Back
			color, 		// Bottom
			color, 		// Left (For C_type==2 only)
			color		// Right (For C_type==2 only)
		);
	C_thickness = new Array(
			4, 		// Front
			4, 	// Top
			4, 	// Back
			2.5, 	// Bottom
			2.5, 	// Left (For C_type==2 only)
			2.5		// Right (For C_type==2 only)
		);
	
	C_sideExpansionOffset = new Array(-1,-1,-1,-1);
	C_housingProfileRightHeight = height_front;
	
	C_depthOfCavitiesForGroovesFront = new Array(0,0,0,0);	// (Top, Bottom, Left, Right). 
	C_depthOfCavitiesForGroovesBack = new Array(0,0,0,0);	// (Top, Bottom, Left, Right). 

	C_assemblyGrooveDepthFront = new Array(1,1,1,1);
	C_assemblyGrooveDepthBack = new Array(1,1,1,1);
}
function preset_test() {
	
	// ENCLOSURE TYPE
	// 1 = based on side sections (results in 4 panels)
	// 2 = based on housing profiles (results in 6 panels)
	C_type = 2;

	// For C_type==1: Side section 1 (42mm high) or 2 (56mm high)?
	C_sideSectionType = 1;

	// For C_type==2: Determines if housing profile 2 is used on a given side.
	// Notice: If set true for a given side, any adjacent sides CANNOT be true as well!
	// For instance, it can be true for Top and Bottom or Left and Right, but not Top and Left
	C_housingProfile2Top = true;
	C_housingProfile2Left = false;
	C_housingProfile2Right = false;
	C_housingProfile2Bottom = true;

	// WIDTH, HEIGHT, DEPTH of enclosure (outer measures):
	// For C_type==1: Height will be overruled by the height of the chosen side section type 1 (42mm) or 2 (56mm).
	C_width = 100;
	C_height = 50;	
	C_depth = 80;

	// Thickness of plates: 2, 2.5, 3, 4
	C_thickness = new Array(
			4, 		// Front
			2.5, 	// Top
			2.5, 	// Back
			2.5, 	// Bottom
			2, 	// Left (For C_type==2 only)
			3		// Right (For C_type==2 only)
		);

	// Colors: These are constants from Schaeffers Scripting API manual
	C_color = new Array(
			elox_blue, 		// Front
			elox_blue, 		// Top
			elox_natural, 	// Back
			elox_black, 		// Bottom
			elox_red, 		// Left (For C_type==2 only)
			elox_blue		// Right (For C_type==2 only)
		);

	// Housing Brackets:   	
	C_numberOfHousingBracketsFront = new Array(1,2,0,0); // (Top, Bottom, Left, Right). Defines the number of housing brackets used on the front for each side panel. -1 will automatically place them pr. 100 mm.
	C_numberOfHousingBracketsBack = new Array(0,0,1,1);  // (Top, Bottom, Left, Right). Defines the number of housing brackets used on the back for each side panel.  -1 will automatically place them pr. 100 mm.
	C_useStudsForHousingBracketsOnFront = true;	// Uses M3x8mm Studs for housing brackets on front (instead of countersunk holes). Notice: This requires you to drill the thread out of the housing brackets on one side and use a washer and nut to fasten it!
	C_useStudsForHousingBracketsOnBack = false;	// Same as above, but for the back.

	// Front and Back Assembly Grooves:
	C_useAssemblyGroovesFront = new Array(true, false, false, false);	// (Top, Bottom, Left, Right). If true, cavities are used on the front panel to fit the top, bottom, left and right plates.
	C_useAssemblyGroovesBack = new Array(false, false, false, false);	// (Top, Bottom, Left, Right). If true, cavities are used on the back panel to fit the top, bottom, left and right plates.
	C_depthOfCavitiesForGroovesFront = new Array(1,1,1,1);	// (Top, Bottom, Left, Right). 
	C_depthOfCavitiesForGroovesBack = new Array(1,1,1,1);	// (Top, Bottom, Left, Right). 
	C_assemblyGrooveDepthFront = new Array(1.5,1.5,1.5,1.5);	// (Top, Bottom, Left, Right). Assembly Groove Depths, front side
	C_assemblyGrooveDepthBack = new Array(1,1,1,1);				// (Top, Bottom, Left, Right). Assembly Groove Depths, back side

	// Ordering
	C_createOrder = false;			// If true, will create an order and add the accessories (profiles, assembly kits etc.)
	C_orderBlackItems = false;		// If true, will order accessories in black instead of natural color.

	// Additional features:
	C_extraFrontWidth = 10;	// If greater than zero, it will add this amount of space on left and right side of the front panel. This is useful to create "rack ears" (see also "C_rackEars" setting). If you want to create a true 19" unit, make sure 2* this value + C_width equals 483mm. A good choice is to set this value at 23mm and C_width = 437 => 23*2+437=483mm
	C_extraFrontHeight = 2;	// If creating a 1U rack unit, 43.60mm is a typical choice for the total height, so add 1.6 in total by setting this value to 0.8. (1U = 44.45mm or 1.75inch)
	C_rackEars = false;			// If true, adds holes for rack mounting. See also "C_extraFrontWidth"
	C_roundFrontCorners = true;	// If true, corners of the front panel are rounded by 2mm which fits the side or housing profiles.
	C_addEdgeGrinding = true;	// You probably want edges grinded on the end profiles (front and back) if you are not making a rack unit! Otherwise they are ugly from tool marks and often very sharp.
	C_addGuideEngravings = true;	// This will add thin, orange engraved lines for preview of where adjacent components (panels, profiles and brackets) are located on each side. Can be useful for positioning other elements after which they should be removed before ordering.

	C_panelNamesPrefix = "";		// A panel name prefix
	C_panelNamesSuffix = "";		// A panel name suffix

	// Exotic features:
	C_sideExpansionOffset = new Array(0,0,0,0); // (Top, Bottom, Left, Right). This offsets the top, bottom, left and right plates outwards if positive. By default the top and bottom plates are placed so their outer surface is flush with the side/housing profiles. When used with housing profile 1 this value should probably be -1!
	C_housingProfileRightHeight = 0;	// Must be less than C_height. If zero = off. Imposes various restrictions on the bottom plate, among other that it has to be bent a bit. Also, cannot be used with housing brackets on the bottom plate (front and back). 
}






/***************************************
 * Configuration value getter-methods
 * These return detailed values for each
 * panel based on the global configuration 
 * variables.
 * Many could potentially be used to provide
 * more fine grained control.
 ***************************************/
var sidesMap = new Array(0,0,0,1,2,3);
function getThickness(sideNum)	{	// SideNum = {1,2,3,4,5,6}
	return C_thickness[sideNum-1];
}
function getColor(sideNum)	{	// SideNum = {1,2,3,4,5,6}
	return C_color[sideNum-1];
}
function assemblyGrooveDepth(sideNum,onBackSide)	{	// SideNum = {2,4,5,6}
	var useIt = onBackSide ? C_useAssemblyGroovesBack[sidesMap[sideNum-1]] : C_useAssemblyGroovesFront[sidesMap[sideNum-1]];
	var value = onBackSide ? C_assemblyGrooveDepthBack[sidesMap[sideNum-1]] : C_assemblyGrooveDepthFront[sidesMap[sideNum-1]];
	return !useIt ? 0 : value;
//	return !useIt ? 0 : getThickness(onBackSide?3:1)-C_frontBackGrooveRemainingMaterial;	// Leave C_frontBackCavityRemainingMaterial mm of material
}
function assemblyGrooveDepthTol(sideNum,onBackSide)	{	// SideNum = {2,4,5,6}
	return 0.1;
}
function assemblyGrooveWidth(sideNum,onBackSide)	{	// SideNum = {2,4,5,6}
	var depthOfCavitiesForGrooves = onBackSide ? C_depthOfCavitiesForGroovesBack[sidesMap[sideNum-1]] : C_depthOfCavitiesForGroovesFront[sidesMap[sideNum-1]];
	return getThickness(sideNum)-depthOfCavitiesForGrooves;
}
function assemblyGrooveWidthTol(sideNum,onBackSide)	{	// SideNum = {2,4,5,6}
	return 0.1;
}
function sideCavityForAssemblyGroovesDepthTolerance(sideNum)	{	// SideNum = {2,4,5,6}
	return 0.1;
}
function sideCavityDepthTolerance(sideNum)	{	// SideNum = {2,4,5,6}
	return 0.1;
}
function getExpansionOffset(sideNum)	{	// SideNum = {2,4,5,6}
	return C_sideExpansionOffset[sidesMap[sideNum-1]];
}
function getNumberOfHousingBrackets(sideNum,onBackSide)	{	// SideNum = {2,4,5,6}
	var num = onBackSide ? C_numberOfHousingBracketsBack[sidesMap[sideNum-1]] : C_numberOfHousingBracketsFront[sidesMap[sideNum-1]];
	return num==-1 ? parseInt(getPanelWidth(sideNum)/100) : num;
}
function useStudsForHousingBrackets(sideNum,onBackSide)	{	// SideNum = {2,4,5,6}
	return onBackSide ? C_useStudsForHousingBracketsOnBack : C_useStudsForHousingBracketsOnFront;
}
function getEdgeProfile(sideNum)	{	// SideNum = {2,4,5,6}
	// Returns a characterization of the side profile or housing profile where it meets the front panel. Widths and heights of it's features (w1,h1,w2,h2,w3,isHousingProfile2)
	// It's imperative that the same profile is used in both sides of any given panel, otherwise the symmetry assumptions in the whole script are broken (which would affect the locations of housing bracket holes for instance.)
	// Also, notice that the configuration of housing profile 2 has other constraints such as that you cannot have profile 2 specified for adjacent sides.
	
	if (C_type==1)	{
		return new Array(10,1.2,7,1.6,10,false);	// Side Sections
	} else {
		var profile2 = false;

		switch(sideNum)	{
			case 2: 	// Top
				profile2 = C_housingProfile2Top;
			break;
			case 4: 	// Bottom
				profile2 = C_housingProfile2Bottom;
			break;
			case 5: 	// Left
				profile2 = C_housingProfile2Left;
			break;
			case 6: 	// Right
				profile2 = C_housingProfile2Right;
			break;
		}	

		if (profile2)	{
			return new Array(0,0,10,2,18,true);		// Housing Profile 2	
		} else {
			return new Array(16,1,10,1.6,13,false);	// Housing Profile 1
		}
	}
}







/************************
 * Helper functions
 * (Helps proces values etc.)
 *************************/

/**
 * For the sides of the enclosure, this returns the offset of the outside perimeter of the box as marked by the side sections or housing profiles surface.
 */
function getPerimeterOffset(sideNum)	{	// sideNum = {2,4,5,6}
	switch(sideNum)	{
		case 2:
		case 4:
			return C_height/2;
		break;
		case 5:
		case 6:
			return C_width/2;
		break;
	}
}
/**
 * For the sides of the enclosure, this returns the offset of the given sides reverse side (facing inwards) relative to the center.
 */
function getReverseSideOffset(sideNum)	{	// sideNum = {2,4,5,6}
	return getFrontSideOffset(sideNum)-getThickness(sideNum);
}
/**
 * For the sides of the enclosure, this returns the offset of the given sides front side (facing outwards) relative to the center.
 */
function getFrontSideOffset(sideNum)	{	// sideNum = {2,4,5,6}
	return getPerimeterOffset(sideNum)+getExpansionOffset(sideNum);
}
/** 
 * Returns the width of the enclosure sides and ends
 * (Return zero for left and right sides in case of side sections are used.)
 */
function getPanelWidth(sideNum)	{	// sideNum = {1,2,3,4,5,6}

	switch(sideNum)	{
		case 1:
			return C_width+C_extraFrontWidth*2;
		break;
		case 3:
			return C_width;
		break;
		case 2:
		case 4:
			var edgeProfileData = getEdgeProfile(sideNum);

			if (sideNum==4 && lowerRightOffset>0)	{
				return Math.sqrt(Math.pow(lowerRightOffset,2)+Math.pow(getPerimeterOffset(5)*2-2*16,2))+5.9*2;
			} else {
				return C_width-edgeProfileData[2]*2;
			}
		break;
		case 5:
		case 6:
			var edgeProfileData = getEdgeProfile(sideNum);
			return C_type==1 ? 0 : C_height-edgeProfileData[2]*2-(sideNum==6?lowerRightOffset:0);
		break;
	}
}
/** 
 * Returns the height of the enclosure sides and ends
 */
function getPanelHeight(sideNum)	{	// sideNum = {1,2,3,4,5,6}

	switch(sideNum)	{
		case 1:
			return C_height+C_extraFrontHeight*2;
		break;
		case 3:
			return C_height;
		break;
		default:
			return C_depth-getThickness(1)-getThickness(3)+assemblyGrooveDepth(sideNum,false)+assemblyGrooveDepth(sideNum,true);
		break;
	}
}
/**
 * This will optimize the radius for cavities to the max and select the largest tool possible (to save costs)
 */
function setRadiusAndToolForCavity(cv)	{
	var width = cv.Width();
	var height = cv.Height();
	
	var min = width<height ? width : height;
	var rad = min/2>1.5 ? 1.5 : min/2;
	cv.SetCornerRadius(rad);
	
	if (rad==1.5)	{
		cv.SetTool(cutter_3_0mm);
	}
}






/**************************
 * Various computations and 
 * initializations.
 **************************/
if (C_type==1)	{
	var C_height = C_sideSectionType==1 ? 42 : 56;
	Print("C_height overruled by side section height, now being: " + C_height + "mm\n");
}
var lowerRightOffset = C_housingProfileRightHeight ? C_height-C_housingProfileRightHeight : 0;
var globalHousingBracketCount = 0;






/************************
 * Creator functions
 * (Creates panels and orders)
 *************************/

/**
 * Create end panels
 *
 * @param	string		Name of panel
 * @param	integer		Location of panel (1=front, 3=back)
 * @return	Frontpanel	Front panel object
 */
function createEnds(name, sideNum) {

	var width = getPanelWidth(sideNum);
	var height = getPanelHeight(sideNum);

		// Create panel:
	var fp = new Frontpanel(C_panelNamesPrefix+name+C_panelNamesSuffix, getThickness(sideNum), width, height, alu_elox);
	fp.SetMaterialColor(getColor(sideNum));
	if (sideNum==3 || C_roundFrontCorners)	{
		fp.SetCornerRadii(2,2,2,2);
	}
	
	if (lowerRightOffset)	{
		var cornerRadius=2;
		var rightHeight = height-lowerRightOffset;
		var leftIndent = 16;

		var alpha = Math.atan((height-rightHeight)/(width-leftIndent-cornerRadius));
		var cornerSinDist = Math.sin(alpha)*cornerRadius;
		var cornerCosDist = Math.cos(alpha)*cornerRadius;

		Print("alpha="+alpha/Math.PI*180+"\n");
//		Print("cornerSinDist="+cornerSinDist+"\n");
//		Print("cornerCosDist="+cornerCosDist+"\n");
		
		var MxO= sideNum==3 ? width : 0;
		var MxF= sideNum==3 ? -1 : 1;

		var dxf = new DxfContour("dxf-canvas1", "", 0, 100, 0);
		dxf.Start(MxO+MxF*0,cornerRadius);
		dxf.LineTo(MxO+MxF*0,height-cornerRadius);
		dxf.ArcToMP(MxO+MxF*(cornerRadius), height, MxO+MxF*(cornerRadius), height-cornerRadius, -1*MxF);
		dxf.LineTo(MxO+MxF*(width-cornerRadius),height);
		dxf.ArcToMP(MxO+MxF*(width), height-cornerRadius, MxO+MxF*(width-cornerRadius), height-cornerRadius, -1*MxF);
		dxf.LineTo(MxO+MxF*(width),(height-rightHeight)+cornerRadius);
		dxf.ArcToMP(MxO+MxF*(width-cornerRadius+cornerSinDist), (height-rightHeight)+cornerRadius-cornerCosDist, MxO+MxF*(width-cornerRadius), (height-rightHeight)+cornerRadius, -1*MxF);
		dxf.LineTo(MxO+MxF*(leftIndent+cornerSinDist), cornerRadius-cornerCosDist);
		dxf.ArcToMP(MxO+MxF*(leftIndent), 0, MxO+MxF*(leftIndent), cornerRadius, -1*MxF);
		dxf.LineTo(MxO+MxF*(cornerRadius), 0);
		dxf.ArcToMP(MxO+MxF*(0), cornerRadius, MxO+MxF*(cornerRadius), cornerRadius, -1*MxF);

		dxf.Finish();
		
		fp.SetBorderContour(dxf);
	}

		// Define center coordinates. These are the reference points for anything because the panels are symmetrical around the center axis
	var centerX = width/2;
	var centerY = height/2;

		// 4 Screw holes (depends on enclosure type!)
	switch(C_type)	{
		case 1: 	// Side Sections:
			var cOffsetX = getPerimeterOffset(5)-4;
			var cOffsetY = getPerimeterOffset(2)-4;

			for(var i=0;i<4;i++)	{	// 0=Upper Right, 1=Upper Left, 2=Lower Right, 3=Lower Left
				var el = new DrillHole("el", 3.20);
				fp.AddElement(el, centerX+cOffsetX*(i&1?-1:1), centerY+cOffsetY*(i&2?-1:1));
				el.SetCountersink(sink_74A_M3);
			}
		break;
		case 2: 	// Housing Profiles
			var cOffsetX = getPerimeterOffset(5)-5.2;
			var cOffsetY = getPerimeterOffset(2)-5.2;

			for(var i=0;i<4;i++)	{	// 0=Upper Right, 1=Upper Left, 2=Lower Right, 3=Lower Left
				var el = new DrillHole("el", 5.00);
				fp.AddElement(el, centerX+cOffsetX*(i&1?-1:1)*(sideNum==3?-1:1), centerY+cOffsetY*(i&2?-1:1)+(i==2?lowerRightOffset:0));
				el.SetCountersink(sink_74A_M5);
			}
		break;
	}

	var sideIndex = new Array(2,4,6,5);	// Top, Bottom, Right, Left (don't change order!)
	var bottomAngle = Math.atan(lowerRightOffset/(getPerimeterOffset(5)*2-2*16))/Math.PI*180;
	//Print(bottomAngle);
	var bottomCutLength = Math.sqrt(Math.pow(lowerRightOffset,2)+Math.pow(getPerimeterOffset(5)*2-2*16,2))+1;
	//Print(bottomCutLength);

		// Assembly Grooves, all sides:
	for(var i=0;i<4;i++)	{ // 0=top, 1=bottom, 2=Right, 3=Left
		var grooveDepth = assemblyGrooveDepth(sideIndex[i], sideNum==3?true:false);
		if (grooveDepth>0 && getPanelWidth(sideIndex[i]))	{	// calling getPanelWidth(sideIndex[i]) makes sure no cavities are made on panels with side sections (function returns 0 for such sides)
			var grooveWidth = assemblyGrooveWidth(sideIndex[i], sideNum==3?true:false);
			var grooveOffset = getReverseSideOffset(sideIndex[i])+grooveWidth/2;

			var el = new Cavity("el");
			if (i<2)	{	// Top/bottom
				fp.AddElement(el, centerX, centerY+grooveOffset*(i&1?-1:1)+(i==1?lowerRightOffset/2:0));
				el.MakeRectangular(getPanelWidth(sideIndex[i])+3, 3.0, grooveDepth + assemblyGrooveDepthTol(sideIndex[i], sideNum==3?true:false), 0.5);
				el.SetHeight(grooveWidth + assemblyGrooveWidthTol(sideIndex[i], sideNum==3?true:false));
				setRadiusAndToolForCavity(el);
				el.PutOnReverseSide();
				if (i==1 && lowerRightOffset > 0)	{
					el.SetAngle(bottomAngle*(sideNum==3?-1:1));
					el.SetWidth(bottomCutLength);

						// Add straight cavities:
					for(var j=0;j<2;j++)	{
						var el = new Cavity("el");
						fp.AddElement(el, centerX+(centerX-8-8/2-1)*(j&1?-1:1), centerY+(grooveOffset*(i&1?-1:1)+(j==(sideNum==3?1:0)?lowerRightOffset:0)));
						el.MakeRectangular(8+2, 3.0, grooveDepth + assemblyGrooveDepthTol(sideIndex[i], sideNum==3?true:false), 0.5);
						el.SetHeight(grooveWidth + assemblyGrooveWidthTol(sideIndex[i], sideNum==3?true:false));
						setRadiusAndToolForCavity(el);
						el.PutOnReverseSide();
					}
				}
			} else {	// Left/Right
				fp.AddElement(el, centerX+grooveOffset*(i&1?-1:1)*(sideNum==3?-1:1), centerY+(i==2?lowerRightOffset/2:0));	//(sideNum==3?-1:1) => Makes sure to mirror left/right on the BACK panel!
				el.MakeRectangular(3.0, getPanelWidth(sideIndex[i])+3, grooveDepth + assemblyGrooveDepthTol(sideIndex[i], sideNum==3?true:false), 0.5);
				el.SetWidth(grooveWidth + assemblyGrooveWidthTol(sideIndex[i], sideNum==3?true:false));
				setRadiusAndToolForCavity(el);
				el.PutOnReverseSide();
			}
		}
	}

		// Housing brackets, all sides
	for(var i=0;i<4;i++)	{ // 0=top, 1=bottom, 2=Right, 3=Left
		var housingBrackets = getNumberOfHousingBrackets(sideIndex[i], sideNum==3?true:false);
		if (housingBrackets>0 && getPanelWidth(sideIndex[i]) && (lowerRightOffset==0 || i!=1))	{	// (lowerRightOffset==0 || i!=1) used to filter the bottom housing profiles out in case they are angled (in the future, housing profiles _could_ be used with proper calculations of placements...)
			var housingBracketDist = getPanelWidth(sideIndex[i]) / (housingBrackets+1);
			var housingBracketStartOffset = housingBracketDist*(housingBrackets-1)/2;
			var housingBracketOffset = getReverseSideOffset(sideIndex[i])-6.5;
			var useStuds = useStudsForHousingBrackets(sideIndex[i], sideNum==3?true:false);
			
			globalHousingBracketCount+=housingBrackets;

			for(var a=0;a<housingBrackets;a++)	{
				if (useStuds)	{
					var el = new Bolt("el", "GU30", 8);
				} else {
					var el = new DrillHole("el", 3.00);
					el.SetCountersink(sink_74A_M3);
				}
				if (i<2)	{	// Top/bottom
					fp.AddElement(el, centerX-housingBracketStartOffset+housingBracketDist*a, centerY+housingBracketOffset*(i&1?-1:1));
					if (C_addGuideEngravings)	{
						fp.AddElement(new Rectangle("el", 10, 10).SetCornerRadius(0).SetTool(engraver_0_2mm).SetColor(engrave_pastel_orange),centerX-housingBracketStartOffset+housingBracketDist*a, centerY+(housingBracketOffset+1.5)*(i&1?-1:1));
					}
				} else {	// Left/Right
					fp.AddElement(el, centerX+housingBracketOffset*(i&1?-1:1)*(sideNum==3?-1:1), centerY-housingBracketStartOffset+housingBracketDist*a);	//(sideNum==3?-1:1) => Makes sure to mirror left/right on the BACK panel!
				}
			}
		}
	}

		// Guide engravings:
	if (C_addGuideEngravings)	{
		for(var i=0;i<4;i++)	{ // 0=top, 1=bottom, 2=Right, 3=Left
			if (i<2)	{	// Top/bottom
				if (getPanelWidth(sideIndex[i]))	{
					fp.AddElement(
						new Rectangle("el", getPanelWidth(sideIndex[i]), getThickness(sideIndex[i])).SetCornerRadius(0).SetTool(engraver_0_2mm).SetColor(engrave_pastel_orange).SetAngle(i==1 && lowerRightOffset > 0 ? bottomAngle*(sideNum==3?-1:1) : 0),
						centerX, 
						centerY+(getFrontSideOffset(sideIndex[i])-getThickness(sideIndex[i])/2)*(i&1?-1:1)+(i==1?lowerRightOffset/2:0)
					);

					fp.AddElement(
						new TextEngraving("el", i==0?"Top":"Bottom").SetAlignment(align_center).SetTextHeight(1).SetColor(engrave_pastel_orange),
						centerX, 
						centerY+(getFrontSideOffset(sideIndex[i])-getThickness(sideIndex[i])/2)*(i&1?-1:1)+(i==1?lowerRightOffset/2:0)
					);
				}
			} else {
				if (getPanelWidth(sideIndex[i]))	{
					fp.AddElement(
						new Rectangle("el", getThickness(sideIndex[i]), getPanelWidth(sideIndex[i])).SetCornerRadius(0).SetTool(engraver_0_2mm).SetColor(engrave_pastel_orange),
						centerX+(getFrontSideOffset(sideIndex[i])-getThickness(sideIndex[i])/2)*(i&1?-1:1)*(sideNum==3?-1:1), 
						centerY+(i==2?lowerRightOffset/2:0)
					);
					fp.AddElement(
						new TextEngraving("el", i==2?"Right":"Left").SetAlignment(align_center).SetTextHeight(1).SetColor(engrave_pastel_orange).SetAngle(90),
						centerX+(getFrontSideOffset(sideIndex[i])-getThickness(sideIndex[i])/2)*(i&1?-1:1)*(sideNum==3?-1:1), 
						centerY+(i==2?lowerRightOffset/2:0)
					);
				}
			}
		}

			// Profiles:
		switch(C_type)	{
			case 1: 	// Side Sections:
				for(var i=0;i<2;i++)	{	// 0=Right, 1=Left
					fp.AddElement(
						new Rectangle("el", 4.5, C_height).SetCornerRadius(0).SetTool(engraver_0_2mm).SetColor(engrave_pastel_orange),
						centerX+(getPerimeterOffset(5)-4.5/2)*(i&1?-1:1), 
						centerY
					);
					for(var j=0;j<2;j++)	{	// 0=Top, 1=Bottom
						fp.AddElement(
							new Rectangle("el", 10, 4.5).SetCornerRadius(0).SetTool(engraver_0_2mm).SetColor(engrave_pastel_orange),
							centerX+(getPerimeterOffset(5)-10/2)*(i&1?-1:1), 
							centerY+(getPerimeterOffset(2)-4.5/2)*(j&1?-1:1)
						);
					}
				}
			break;
			case 2: 	// Housing Profiles
				for(var i=0;i<4;i++)	{ // 0=top, 1=bottom, 2=Right, 3=Left
					var edgeProfileData = getEdgeProfile(sideIndex[i]);
					var profileSideWidth = edgeProfileData[5]?18:16;
					var profileSideHeight = edgeProfileData[5]?4.7+2:5.2+1;
					if (i<2)	{
						for(var j=0;j<2;j++)	{	// 0=Right, 1=Left
							fp.AddElement(
								new Rectangle("el", profileSideWidth, profileSideHeight).SetCornerRadius(0).SetTool(engraver_0_2mm).SetColor(engrave_pastel_orange),
								centerX+(getPerimeterOffset(5)-profileSideWidth/2)*(j&1?-1:1)*(sideNum==3?-1:1), 
								centerY+(getPerimeterOffset(2)-profileSideHeight/2)*(i&1?-1:1)+(i==1&&j==0?lowerRightOffset:0)
							);
						}
					} else {
						for(var j=0;j<2;j++)	{	// 0=Top, 1=Bottom
							fp.AddElement(
								new Rectangle("el",profileSideHeight, profileSideWidth).SetCornerRadius(0).SetTool(engraver_0_2mm).SetColor(engrave_pastel_orange),
								centerX+(getPerimeterOffset(5)-profileSideHeight/2)*(i&1?-1:1)*(sideNum==3?-1:1), 
								centerY+(getPerimeterOffset(2)-profileSideWidth/2)*(j&1?-1:1)+(i==2&&j==1?lowerRightOffset:0)
							);
						}
					}
				}
			break;
		}
	}
	
		// FRONT special: D-holes for rack mounting:
	if (C_rackEars && sideNum==1)	{
		var cOffsetX = centerX-9.00;
		var cOffsetY = centerY-5.95;

		for(var i=0;i<4;i++)	{
			var el = new DHole("el", dhole_double, 10.50, 7.60);
			fp.AddElement(el, centerX+cOffsetX*(i&1?-1:1), centerY+cOffsetY*(i&2?-1:1));
			el.SetAngle(90);
		}
	}
	
		// Edge grinding
	if (C_addEdgeGrinding)	{
		fp.SetRemark("Please grind the edges.");
	}
		
	fp.UnsetModified();	
	return fp;	
}

/**
 * Creates top, bottom, left and right side panels
 *
 * @param	string		Name of panel
 * @param	integer		Location of panel (2=top, 4=bottom, 5=left, 6=right)
 * @return	Frontpanel	Front panel object
 */
function createSides(name, sideNum)	{

	// Principle: The external side of these panels lie flush with side sections except offset by C_topBottomExpansionOffset
	
	var width = getPanelWidth(sideNum);
	var height = getPanelHeight(sideNum);

		// Create panel:
	var fp = new Frontpanel(C_panelNamesPrefix+name+C_panelNamesSuffix, getThickness(sideNum), width, height, alu_elox);
	fp.SetMaterialColor(getColor(sideNum));

		// Define center coordinates. These are the reference points for anything because the panels are symmetrical around the center axis		
	var centerX = width/2;
	var centerY = height/2;
	var edgeProfileData = getEdgeProfile(sideNum);

		// Cavities on sides for side sections and housing profiles (both top and bottom cavities are possible outcomes)
	for(var i=0;i<2;i++)	{	// i=0: Left, i=1: Right

		var upperCavityDepth = getFrontSideOffset(sideNum) - (getPerimeterOffset(sideNum)-edgeProfileData[1]);
		var underCavityDepth = (getPerimeterOffset(sideNum)-edgeProfileData[1]-edgeProfileData[3]) - getReverseSideOffset(sideNum);

		if (upperCavityDepth>0)	{
			var grooveDepth = edgeProfileData[0]-edgeProfileData[2];
			var cavWidth = grooveDepth+0.5;
			var cavOffset = (centerX-grooveDepth+cavWidth/2);
			
			var el = new Cavity("el");
			fp.AddElement(el, centerX+cavOffset*(i&1?-1:1), centerY);
			el.MakeRectangular(cavWidth, height+4, upperCavityDepth + sideCavityDepthTolerance(sideNum), 1.5);
			setRadiusAndToolForCavity(el);
		}
	 	if (underCavityDepth>0)	{
			var grooveDepth = edgeProfileData[4]-edgeProfileData[2];
			var cavWidth = grooveDepth+0.5;
			var cavOffset = (centerX-grooveDepth+cavWidth/2);

			if (sideNum==4 && lowerRightOffset > 0)	{	// For consoles. Assuming only UNDER cavities!
				cavWidth = 6+0.5;
				cavOffset = (centerX-6+cavWidth/2);
			}
			
			var el = new Cavity("el");
			fp.AddElement(el, centerX+cavOffset*(i&1?-1:1), centerY);
			el.MakeRectangular(cavWidth, height+4, underCavityDepth + sideCavityDepthTolerance(sideNum), 1.5);
			setRadiusAndToolForCavity(el);
			el.PutOnReverseSide();

			if (sideNum==4 && lowerRightOffset > 0)	{	// For consoles. Assuming only UNDER cavities! This is the folding cavity
				var el = new Cavity("el");
				fp.AddElement(el, centerX+(centerX-6)*(i&1?-1:1), centerY);
				el.MakeRectangular(3, height+4, getThickness(sideNum)-0.5, 0.5);
				el.SetWidth(1);
				setRadiusAndToolForCavity(el);
				el.PutOnReverseSide();
			}
		}
		
		if (edgeProfileData[5])	{	// If housing profile 2, add screw holes:
			var numberOfScrewHoles = parseInt(height/100)+1;
			var screwHoleDist = height / numberOfScrewHoles;
			var screwHoleStartOffset = screwHoleDist*(numberOfScrewHoles-1)/2;
			
			for(var a=0;a<numberOfScrewHoles;a++)	{
				var el = new DrillHole("el", 3.00);
				fp.AddElement(el, centerX+(centerX-4)*(i&1?-1:1), centerY-screwHoleStartOffset+screwHoleDist*a);
				el.SetCountersink(sink_74A_M3);
			}
		}
	}

		// Cavity for Front and Back Assembly Grooves:
	for(var i=0;i<2;i++)	{	// i=0:front, i=1: back
		var grooveDepth = assemblyGrooveDepth(sideNum, i);
		if (grooveDepth)	{
			var cavDepth = getThickness(sideNum)-assemblyGrooveWidth(sideNum, i);
			if (cavDepth>0)	{
				var cavHeight = grooveDepth+0.5;
				if (cavHeight<3)	cavHeight=3;
				var cavOffset = centerY-grooveDepth+cavHeight/2;
				var el = new Cavity("el");
				fp.AddElement(el, centerX, centerY+cavOffset*(i&1?1:-1));
				el.MakeRectangular(width+4, cavHeight, cavDepth + sideCavityForAssemblyGroovesDepthTolerance(sideNum), 1.5);
				setRadiusAndToolForCavity(el);
			}
		}
	}

		// Housing brackets:
	if (lowerRightOffset==0 || sideNum!=4)	{
		for(var i=0;i<2;i++)	{	// i=0:front, i=1: back
			var housingBrackets = getNumberOfHousingBrackets(sideNum, i);
			if (housingBrackets>0)	{
				var housingBracketDist = width / (housingBrackets+1);
				var housingBracketStartOffset = housingBracketDist*(housingBrackets-1)/2;
				var housingBracketOffset = centerY-assemblyGrooveDepth(sideNum, i)-6.5;

				for(var a=0;a<housingBrackets;a++)	{
					var el = new DrillHole("el", 3.00);
					el.SetCountersink(sink_74A_M3);
					fp.AddElement(el, centerX-housingBracketStartOffset+housingBracketDist*a, centerY+housingBracketOffset*(i&1?1:-1));	// (i&1?1:-1) is "reversed" compared to createSides() but this is on purpose to make sure the "top" side of the panel matches the "back" of the enclosure.
					if (C_addGuideEngravings)	{
						fp.AddElement(new Rectangle("el", 10, 10).SetCornerRadius(0).SetTool(engraver_0_2mm).SetColor(engrave_pastel_orange),centerX-housingBracketStartOffset+housingBracketDist*a, centerY+(housingBracketOffset+1.5)*(i&1?1:-1));
					}
				}
			}
		}
	}
		
	if (C_addGuideEngravings)	{
		
			// Front:
		fp.AddElement(
			new Rectangle("el", sideNum>=5?getPanelHeight(1):getPanelWidth(1), getThickness(1)).SetCornerRadius(0).SetTool(engraver_0_2mm).SetColor(engrave_pastel_orange),
			centerX,
			centerY-(centerY-assemblyGrooveDepth(sideNum, false)+getThickness(1)/2)
		);
		fp.AddElement(
			new TextEngraving("el","Front").SetAlignment(align_center).SetTextHeight(1).SetColor(engrave_pastel_orange),
			centerX,
			centerY-(centerY-assemblyGrooveDepth(sideNum, false)+getThickness(1)/2)+5
		);
		
			// Back:
		fp.AddElement(
			new Rectangle("el", sideNum>=5?getPanelHeight(3):getPanelWidth(3), getThickness(3)).SetCornerRadius(0).SetTool(engraver_0_2mm).SetColor(engrave_pastel_orange),
			centerX,
			centerY+(centerY-assemblyGrooveDepth(sideNum, true)+getThickness(3)/2)
		);
		fp.AddElement(
			new TextEngraving("el","Back").SetAlignment(align_center).SetTextHeight(1).SetColor(engrave_pastel_orange),
			centerX,
			centerY+(centerY-assemblyGrooveDepth(sideNum, true)+getThickness(3)/2)-5
		);
		
			// Profiles:
		switch(C_type)	{
			case 1: 	// Side Sections:
				var profileSideWidth = 10;
			break;
			case 2: 	// Housing Profiles:
				var profileSideWidth = edgeProfileData[5]?18:16;
			break;
		}
		for(var i=0;i<2;i++)	{	// 0=Right, 1=Left
			fp.AddElement(
				new Rectangle("el", profileSideWidth, height-assemblyGrooveDepth(sideNum,false)-assemblyGrooveDepth(sideNum,true)).SetCornerRadius(0).SetTool(engraver_0_2mm).SetColor(engrave_pastel_orange),
				centerX+(centerX+edgeProfileData[2]-profileSideWidth/2)*(i&1?-1:1), 
				centerY+(assemblyGrooveDepth(sideNum,false)-assemblyGrooveDepth(sideNum,true))/2
			);
		}		
	}
		
	fp.UnsetModified();	
	return fp;
}

/**
 * Create order
 */
function createOrder()	{
	var profileLengths = C_depth-getThickness(1)-getThickness(3);
	switch(C_type)	{
		case 1:
			var itemID = "GLGP10"+C_sideSectionType+(C_orderBlackItems?"3":"1");
			OrderAddItemWithLength(itemID, 2, profileLengths);	// Side Sections

			OrderAddItem("GGMS11"+(C_orderBlackItems?"2":"1")+"2",1);	// Assembly Kit
		break;
		case 2:
			var profile2Cnt = 
				(C_housingProfile2Top?1:0)+
				(C_housingProfile2Left?1:0)+
				(C_housingProfile2Right?1:0)+
				(C_housingProfile2Bottom?1:0);
				
			if (profile2Cnt<=1)	{	// Only one side or none has housing profiles 2:
				var itemID = "GLGP201"+(C_orderBlackItems?"3":"1");
				OrderAddItemWithLength(itemID, profile2Cnt?2:4, profileLengths);	// Housing Profiles 1
			}
			if (profile2Cnt)	{	// There are housing profiles 2 involved:
				var itemID = "GLGP202"+(C_orderBlackItems?"3":"1");
				OrderAddItemWithLength(itemID, profile2Cnt>=2?4:2, profileLengths);	// Housing Profiles 2
				
					// Add small screws for side mounted on housing profiles 2 (must be M3x5, not longer!)
				OrderAddItem("GGMS17"+(C_orderBlackItems?"5":"4")+"2", 1);	// Square nuts
			}

			OrderAddItem("GGMS16"+(C_orderBlackItems?"2":"1")+"2",1);	// Assembly Kit
		break;
	}
	
		// Edge Grinding:
	if (C_addEdgeGrinding)	{
		OrderSetRemark("Notice, there are edge grindings on some panels.");
	}
	
		// Housing brackets:
	if (globalHousingBracketCount>0)	{
		if (globalHousingBracketCount<=4)	{
			OrderAddItem("GGWS0111",1);	// Housing brackets x4
		} else if (globalHousingBracketCount<=8)	{
			OrderAddItem("GGWS0112",1);	// Housing brackets x8
		} else if (globalHousingBracketCount<=12)	{
			OrderAddItem("GGWS0113",1);	// Housing brackets x12
		} else {
			OrderAddItem("GGWS0114",Math.ceil(globalHousingBracketCount/25));	// Housing brackets x25
		}
	}
}

// Rotation 90 degress both ways:
function rotatePanel(fp,rotation)	{
	var width = fp.Width();
	var height = fp.Height();
	
	var els = fp.Elements();
	switch(rotation)	{
		case 90:
			fp.SetWidth(height);
			fp.SetHeight(width);
			
			els.forEach(function(el){
				el.MoveTo(height-el.Y(),el.X());
				el.Rotate(90);
			});
		break;
		case -90:
		case 270:
			fp.SetWidth(height);
			fp.SetHeight(width);

			els.forEach(function(el){
				el.MoveTo(el.Y(),width-el.X());
				el.Rotate(-90);
			});
		break;
	}
}




// ###MARKER_PREMAKE###



/**************************
 * Execute...
 **************************/

// Front (1)
var fp1 = createEnds("Front",1);
AddFrontpanel(fp1);

//rotatePanel(fp1,90);

// Top (2)
var fp2 = createSides("Top",2);
AddFrontpanel(fp2);

// Back (3)
var fp3 = createEnds("Back",3);
AddFrontpanel(fp3);

// Bottom (4)
var fp4 = createSides("Bottom",4);
AddFrontpanel(fp4);

if (C_type==2)	{
	// Left (5)
	var fp5 = createSides("Left",5);
	AddFrontpanel(fp5);

	// Right (6)
	var fp6 = createSides("Right",6);
	AddFrontpanel(fp6);
}



// Create order with accessories:
if (C_createOrder)	{
	createOrder();
}








/***************************************************************
*  (c) 2013 SKAARHOJ Engineering, www.skaarhoj.com
*  Author: Kasper Skårhøj <kasper@skaarhoj.com>
*  All rights reserved
*
*  This script is free software; you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation; either version 2 of the License, or
*  (at your option) any later version.
*
*  The GNU General Public License can be found at
*  http://www.gnu.org/copyleft/gpl.html.
*
*  This script is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  This copyright notice MUST APPEAR in all copies of the script!
***************************************************************/

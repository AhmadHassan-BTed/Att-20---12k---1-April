package main

import (
	"fmt"
	"image"
	"image/png"
	"log"
	"os"
	"path/filepath"

	"github.com/DabDavis/eqoa-esf-tools/pkg/esf"
)

// All 252 entries from GetUITexture table at 0x004EDAF8 (sheet 1 only)
type atlasEntry struct {
	texID  int
	x, y   int
	w, h   int
	desc   string
}

// Pieces around the face indicator area (Y=270-380, X=0-70) plus known pieces
var pieces = []atlasEntry{
	// Face indicators
	{115, 44, 329, 23, 21, "face_hostile"},
	{116, 46, 309, 19, 19, "face_neutral"},
	{117, 45, 289, 21, 19, "face_friendly"},
	// Panel/window pieces near face indicators
	{29, 4, 300, 36, 31, "panel_bg_tile"},
	{131, 207, 263, 46, 57, "window_corner_TL"},
	{132, 254, 263, 46, 57, "window_corner_TR"},
	{133, 207, 321, 46, 57, "window_corner_BL"},
	{134, 255, 321, 49, 48, "window_corner_BR"},
	// Compass
	{6, 0, 381, 59, 69, "compass_bezel"},
	{129, 24, 243, 44, 44, "compass_rose"},
	// Bars
	{80, 69, 204, 52, 44, "bar_frame_round"},
	{136, 123, 178, 60, 10, "dark_strip"},
	{151, 123, 167, 60, 6, "dark_inset"},
	{7, 123, 189, 55, 6, "bar_hp_red"},
	{8, 123, 195, 55, 6, "bar_power_gold"},
	{9, 123, 201, 55, 6, "bar_xp_blue"},
	{135, 208, 248, 104, 15, "separator_ornate"},
	// Spell/button
	{239, 377, 385, 25, 25, "spell_slot"},
	{65, 64, 379, 17, 16, "arrow_up"},
	{67, 64, 397, 17, 16, "arrow_down"},
	// Icons
	{120, 122, 208, 33, 32, "shield_icon"},
	{122, 4, 185, 34, 32, "bag_icon"},
	// Window frames
	{63, 207, 168, 73, 34, "window_titlebar"},
	{64, 281, 168, 12, 34, "window_titlebar_cap"},
	{77, 207, 203, 46, 44, "tab_bar_left"},
	{78, 305, 203, 16, 44, "tab_bar_right"},
	// Decorative
	{59, 0, 0, 31, 184, "celtic_knot_border"},
	// Checkbox/radio
	{73, 103, 378, 24, 24, "checkbox_unchecked"},
	{74, 130, 378, 24, 24, "checkbox_checked"},
}

func scalePS2Alpha(img *image.NRGBA) *image.NRGBA {
	bounds := img.Bounds()
	out := image.NewNRGBA(bounds)
	copy(out.Pix, img.Pix)
	for i := 3; i < len(out.Pix); i += 4 {
		a := int(out.Pix[i])
		a = a * 255 / 128
		if a > 255 {
			a = 255
		}
		out.Pix[i] = byte(a)
	}
	return out
}

func savePNG(img image.Image, path string) error {
	f, err := os.Create(path)
	if err != nil {
		return err
	}
	defer f.Close()
	return png.Encode(f, img)
}

func main() {
	assetsDir := "/home/sdg/eqoa-assets"
	if len(os.Args) > 1 {
		assetsDir = os.Args[1]
	}
	outDir := "/tmp/atlas_pieces"
	os.MkdirAll(outDir, 0755)

	uiPath := filepath.Join(assetsDir, "UI.ESF")
	uiFile, err := esf.Open(uiPath)
	if err != nil {
		log.Fatalf("open UI.ESF: %v", err)
	}
	if err := uiFile.BuildDictionary(); err != nil {
		log.Fatalf("build dict: %v", err)
	}

	const uiAtlasDictID int32 = -127886075
	atlasObj, err := uiFile.FindObject(uiAtlasDictID)
	if err != nil || atlasObj == nil {
		log.Fatalf("atlas not found: %v", err)
	}
	atlasSurf, ok := atlasObj.(*esf.Surface)
	if !ok || atlasSurf == nil || atlasSurf.Image == nil {
		log.Fatalf("atlas is not a Surface")
	}

	atlas := scalePS2Alpha(atlasSurf.Image)
	log.Printf("Atlas: %dx%d", atlas.Bounds().Dx(), atlas.Bounds().Dy())

	// Save full atlas
	savePNG(atlas, filepath.Join(outDir, "full_atlas.png"))
	log.Printf("Saved full_atlas.png")

	// Save crops around face indicator area for identification
	crops := []struct {
		name           string
		x, y, x2, y2  int
	}{
		{"face_area_wide", 0, 260, 80, 380},
		{"left_of_faces_top", 0, 270, 44, 300},
		{"left_of_faces_mid", 0, 300, 44, 340},
		{"left_of_faces_bot", 0, 340, 44, 380},
		{"above_faces", 0, 250, 70, 290},
		{"below_faces", 0, 340, 70, 400},
		// Also scan the full left column
		{"left_col_184_260", 0, 184, 70, 260},
		{"left_col_260_340", 0, 260, 70, 340},
		{"left_col_340_420", 0, 340, 70, 420},
		// Bottom-left quadrant wide view
		{"bottom_left_quadrant", 0, 256, 210, 450},
		// Above stone busts (tex131/132 at Y=263, X=207-300)
		{"above_busts_wide", 200, 240, 320, 265},
		{"above_busts_tight", 207, 248, 312, 263},
		// Wider area above busts
		{"above_busts_area", 120, 230, 330, 270},
		// Left of face indicators
		{"left_of_faces_precise_a", 0, 270, 44, 290},
		{"left_of_faces_precise_b", 0, 260, 44, 280},
		// Scroll border piece — left of blue face (tex117 at 45,289)
		{"scroll_border_a", 0, 286, 44, 296},
		{"scroll_border_b", 0, 288, 44, 298},
		{"scroll_border_c", 4, 287, 42, 295},
		{"scroll_border_d", 0, 285, 45, 300},
		{"scroll_border_e", 2, 289, 44, 297},
		// Bottom-left corner of atlas
		{"bottom_left_corner", 0, 430, 200, 512},
		{"bottom_left_corner_wide", 0, 400, 250, 512},
		{"bottom_left_rows", 0, 450, 200, 512},
		// 2nd+3rd column, bottom 2 rows — isolate pieces
		{"bl_col23_row12", 32, 460, 100, 512},
		{"bl_col23_row12_wide", 28, 450, 110, 512},
		{"bl_col2_bottom", 32, 480, 64, 512},
		{"bl_col3_bottom", 64, 480, 100, 512},
		{"bl_col2_above", 32, 460, 64, 480},
		{"bl_col3_above", 64, 460, 100, 480},
		// Individual dark panel pieces from 2x2 grid at ~(32,480)
		{"panel_piece_TL", 32, 480, 48, 496},
		{"panel_piece_TR", 48, 480, 64, 496},
		{"panel_piece_BL", 32, 496, 48, 512},
		{"panel_piece_BR", 48, 496, 64, 512},
		// Slightly wider crops in case bounds are off
		{"panel_piece_TL_w", 31, 479, 50, 498},
		{"panel_piece_TR_w", 47, 479, 66, 498},
		{"panel_piece_BL_w", 31, 495, 50, 513},
		{"panel_piece_BR_w", 47, 495, 66, 513},
		// Full 2x2 grid tight
		{"panel_2x2_grid", 31, 479, 66, 513},
	}
	for _, cr := range crops {
		sub := atlas.SubImage(image.Rect(cr.x, cr.y, cr.x2, cr.y2))
		savePNG(sub, filepath.Join(outDir, cr.name+".png"))
		log.Printf("Saved %s.png (%d,%d → %d,%d)", cr.name, cr.x, cr.y, cr.x2, cr.y2)
	}

	// Save each known piece
	for _, p := range pieces {
		sub := atlas.SubImage(image.Rect(p.x, p.y, p.x+p.w, p.y+p.h))
		name := fmt.Sprintf("tex%03d_%s.png", p.texID, p.desc)
		savePNG(sub, filepath.Join(outDir, name))
		log.Printf("Saved %s (%dx%d at %d,%d)", name, p.w, p.h, p.x, p.y)
	}

	log.Printf("Done — %d pieces saved to %s", len(pieces)+2, outDir)
}

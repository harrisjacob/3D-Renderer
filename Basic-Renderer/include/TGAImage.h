#include <cstdint>
#include <fstream>
#include <vector>

//Align struct members every available byte
#pragma pack(push, 1)

struct TGA_Header {
	std::uint8_t idlength{};
	std::uint8_t colormaptype{};
	std::uint8_t datatypecode{};
	std::uint16_t colormaporigin{};
	std::uint16_t colormaplength{};
	std::uint8_t colormapdepth{};
	std::uint16_t x_origin{};
	std::uint16_t y_origin{};
	std::uint16_t width{};
	std::uint16_t height{};
	std::uint8_t bitsperpixel{};
	std::uint8_t imagedescriptor{};
};

//Remove alignment requirement
#pragma pack(pop)

//Struct used for parsing the data TGA file
struct TGAColor {

	std::uint8_t bgra[4] = {0,0,0,0}; //Bytes are store low to high, hence backward ordering
	std::uint8_t bytesperpixel{};

	//Default constructor
	TGAColor() = default;
	//Constructor with value intialization
	TGAColor(const std::uint8_t _r, const std::uint8_t _g , const std::uint8_t _b, std::uint8_t _a=255)
		:bgra{_b,_g,_r,_a}, bytesperpixel(4) {}

	//Constructor that makes a copy, not a copy constructor
	TGAColor(const std::uint8_t *src, const std::uint8_t _bpp) : bgra{0,0,0,0}, bytesperpixel(_bpp){
		for(int i=0;i<_bpp;i++)
			bgra[i] = src[i];
	}

	//Overload operator to select color component
	std::uint8_t& operator[](const int i) { return bgra[i]; }


	//Overload operator to adjust the color intensity
	TGAColor operator*(const double intensity) const{
		TGAColor result = *this;
		double clamped = std::max(0.0, std::min(intensity, 1.0));
		for(int i=0; i<4; i++) result.bgra[i] = bgra[i]*clamped;
		return result;
	}

};


class TGAImage {
protected:
	std::vector<std::uint8_t> data;
	int width;
	int height;
	int bytesPerPixel;

	bool load_rle_data(std::ifstream&);
	bool unload_rle_data(std::ofstream &) const;

public:
	enum Format { GRAYSCALE=1, RGB=3, RGBA=4};

	TGAImage();
	TGAImage(const int, const int, const int);
	bool read_tga_file(const std::string);
	bool write_tga_file(const std::string, const bool =true, const bool =true) const;
	void flip_horizontally();
	void flip_vertically();
	void scale(const int, const int);
	TGAColor get(const int, const int) const;
	void set(const int, const int, const TGAColor &);
	int get_width() const;
	int get_height() const;
	int get_bytespp();
	std::uint8_t* buffer();
	void clear();
	void getInfo();
};

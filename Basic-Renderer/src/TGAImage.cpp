#include <algorithm>
#include <iostream>
#include <cstring>


#include "TGAImage.h"

//Default constructor
TGAImage::TGAImage(): data(), width(0), height(0), bytesPerPixel(0){}

//Constructor by value
TGAImage::TGAImage(const int _width, const int _height, const int _bpp) :
				data(_width*_height*_bpp, 0), width(_width), height(_height), bytesPerPixel(_bpp) {}


//Read in a TGA file
bool TGAImage::read_tga_file(const std::string filename){
	std::ifstream in;
	in.open(filename, std::ios::binary);
	if(!in.is_open()){
		std::cerr << "Can't open file " << filename << "\n";
		in.close();
		return false;
	}

	//Retrieve the TGA header
	TGA_Header header;
	//Bytes are packed sequentially so read sizeof(header) bytes into the casted char pointer
	in.read(reinterpret_cast<char *>(&header), sizeof(header));
	if(!in.good()){
		std::cerr << "An error occured while reading the header\n";
		in.close();
		return false;
	}

	//read values from the header
	width = header.width;
	height = header.height;
	bytesPerPixel = header.bitsperpixel >> 3;	//Convert bitsPerPixel to bytesPerPixel


	if(width <=0){
		in.close();
		std::cerr << "Bad width value\n";
		return false;
	}

	if(height <=0){
		in.close();
		std::cerr << "Bad height value\n";
		return false;
	}

	if(bytesPerPixel!=GRAYSCALE && bytesPerPixel!=RGB && bytesPerPixel!=RGBA){
		in.close();
		std::cerr << "Bad bytesPerPixel value. Read " << bytesPerPixel << " bytes per pixel\n";
		return false;
	}

	size_t nbytes = bytesPerPixel*width*height;
	data = std::vector<std::uint8_t>(nbytes, 0);

	//For infomation on header codes see: https://en.wikipedia.org/wiki/Truevision_TGA#Header
	if(header.datatypecode==3 || header.datatypecode==2){ //uncompressed (3=grayscale, 2=true color)

		//Read in the raw data
		in.read(reinterpret_cast<char *>(data.data()),nbytes);
		if(!in.good()){
			in.close();
			std::cerr << "An error occured while reading the raw data\n";
			return false;
		}

	} else if(header.datatypecode==10 || header.datatypecode==11){ //run length encoded (10=true color, 11=grayscale)
	
		if(!load_rle_data(in)){
			in.close();
			std::cerr << "An error occured while reading the run length encoded data\n";
			return false;
		}


	} else {
		in.close();
		std::cerr << "Failed to read data. Unknown file format of type " << (int)header.datatypecode << "\n";
		return false;
	}


	//For orientation see: https://www.dca.fee.unicamp.br/~martino/disciplinas/ea978/tgaffs.pdf pg 9

	//Objective read the data in assuming top left corner origin orientation

	//Byte 5 of the image descriptor is the vertical orientation
	//If origin at the bottom, flip to the top
	if(!(header.imagedescriptor & 0x20)){ //(header.imagedescriptor[5]: 1=top, 0=bottom)
		flip_vertically();
	}
	//Note: the image will get flipped before it is drawn



	//Byte 4 of the image descriptor is the horizontal orientation
	if(header.imagedescriptor & 0x10){ //(header.imagedescriptor[4]: 1=right, 0=left)
		flip_horizontally();
	}

	//Orientation should now be upper left

	in.close();
	return true;
}

//Function used to load a run length encoded (rle) TGA file
bool TGAImage::load_rle_data(std::ifstream& in){

	size_t pixelCount = width*height;			//Expected pixel to read
	size_t currentPixel = 0;					//Should always maintain number of pixels read and compare to expected pixels (pixelCount)
	size_t currentByte = 0;						//Should always maintain offset into the TGAImage data field
	TGAColor colorBuffer;

	do{
		std::uint8_t chunkHeader = 0;
		chunkHeader = in.get();	//Read a single byte from the input stream

		if(!(in.good())){
			std::cerr << "An error occured when parsing the run length encoded data\n";
			return false;
		}
	
		//See http://www.paulbourke.net/dataformats/tga/ for more information on encoding packets
		//High order bit is 1 for run length packet and 0 for raw packet
		
		if(chunkHeader < 128){ //Raw packet (high order bit is 0, hence less than 128)
			//The 7 least significant bits of the header specify the number of colors that follow the header
			
			chunkHeader++;

			for(int i=0; i<chunkHeader; i++){
				in.read(reinterpret_cast<char*>(colorBuffer.bgra), bytesPerPixel);	//Read a full pixel into a buffer
				if(!in.good()){
					std::cerr << "An error occured while reading in a raw packet\n";
					getInfo();
					std::cerr << "Failed on pixel " << currentPixel << " of " << pixelCount;
					std::cerr << " (Row " << currentPixel/width << " pixel " << currentPixel%width << ")\n";
					return false;
				}

				//Copy the color from the buffer into the TGAImage object data field
				for(int t=0; t<bytesPerPixel; t++)
					data[currentByte++] = colorBuffer.bgra[t];


				currentPixel++;
				if(currentPixel > pixelCount){
					std::cerr << "Too many pixels were read while reading raw packets\n";
					return false;
				}

			}



		}else{ //Run length packet (high order bit is 1, so will be 128 or greater)
			
			//Header value is followed by a single color value that occurs consecutively N times
			//N is specified by the lower 7 bits of the header 

			chunkHeader -= 127; //Disregard high order bit to obtain lower 7 bits of count

			//Read in the color that is occurse N times consecutively
			in.read(reinterpret_cast<char*>(colorBuffer.bgra), bytesPerPixel);
			if(!in.good()){
				std::cerr << "An error occured while reading in a run length packet\n";
				return false;
			}

			for(int i=0; i<chunkHeader; i++){
				for(int t=0; t<bytesPerPixel; t++){
					data[currentByte++] = colorBuffer.bgra[t];
				}
			}

			currentPixel++;
			if(currentPixel > pixelCount){
				std::cerr << "Too many pixels were read while reading rune length packets\n";
				return false;
			}

		}

	} while(currentPixel < pixelCount);

	return true;

}

//Writes data stream to  a TGA file
bool TGAImage::write_tga_file(const std::string filename, const bool vflip, const bool rle) const {

	std::uint8_t developer_area_ref[4] = {0};
	std::uint8_t extension_area_ref[4] = {0};
	std::uint8_t footer[18] = {'T','R','U','E','V','I','S','I','O','N','-','X','F','I','L','E','.','\0'};
	std::ofstream out;
	out.open(filename, std::ios::binary);
	if(!out.is_open()){
		std::cerr << "cant open file " << filename << " for writing\n";
		out.close();
		return false;
	}

	TGA_Header header;
	header.bitsperpixel = bytesPerPixel << 3; //Convert to bits
	header.width = width;
	header.height = height;
	header.datatypecode = (bytesPerPixel==GRAYSCALE ? (rle?11:3):(rle?10:2));
	header.imagedescriptor = vflip ? 0x00 : 0x20;
	out.write(reinterpret_cast<const char*>(&header), sizeof(header)); //Write the header
	if(!out.good()){
		out.close();
		std::cerr << "Failed to dump the header to tga file\n";
		return false;
	}
	if(!rle){
		out.write(reinterpret_cast<const char*>(data.data()), width*height*bytesPerPixel);
		if(!out.good()){
			std::cerr << "Failed to write raw data to tga file\n";
			out.close();
			return false;
		}
	}else{
		if(!unload_rle_data(out)){
			out.close();
			std::cerr << "Failed ot unload rle data to tga file\n";
			return false;
		}
	}

	out.write(reinterpret_cast<const char *>(developer_area_ref), sizeof(developer_area_ref));
	if(!out.good()){
		std::cerr << "Failed to write developer area to tga file\n";
		out.close();
		return false;
	}

	out.write(reinterpret_cast<const char *>(extension_area_ref), sizeof(extension_area_ref));
	if(!out.good()){
		std::cerr << "Failed to write extension area to tga file\n";
		out.close();
		return false;
	}
	out.write(reinterpret_cast<const char *>(footer), sizeof(footer));
	if(!out.good()){
		std::cerr << "Failed to write footer to tga file\n";
		out.close();
		return false;
	}


	out.close();
	return true;

}


//Write encoding to output file
bool TGAImage::unload_rle_data(std::ofstream &out) const{

	const std::uint8_t max_chunk_length = 128;
	size_t nPixels = width*height;
	size_t currentPixel;
	while(currentPixel < nPixels){

		size_t chunk_start = currentPixel*bytesPerPixel;
		size_t currentByte = currentPixel*bytesPerPixel;
		std::uint8_t run_length = 1;
		bool raw = true;

		//Find a chunk
		while(currentPixel+run_length < nPixels && run_length < max_chunk_length){
			bool nextMatches = true;
			for(int t=0; nextMatches && t<bytesPerPixel; t++)
				nextMatches = (data[currentByte + t] == data[currentByte+t+bytesPerPixel]); //Check if the current pixel matches the previous pixel by comparing its RGB values

			currentByte += bytesPerPixel; 					//Move forward one pixel
			
			if(run_length == 1){							//If this is the first pixel in the run of pixels
				raw = !nextMatches;							//If the pixel matches the next pixel, don't use a raw packet
			}

			if(raw && nextMatches){							//Raw packets should not have consecutive matching pixels		
				run_length--;								//Back up one pixel
				break;
			}

			if(!raw && !nextMatches){						//Run length packets (non-raw packets) should have consecutive matching pixels
				break;										//If the next doesn't match, leave it for the next packet
			}

			run_length++;
		}

		currentPixel += run_length;							//Move the currentPixel forward the number of pixels handled in the current packet


		//See http://www.paulbourke.net/dataformats/tga/ for more information on encoding packets
		//Write the packet header
		out.put(raw?run_length-1:run_length+127);
		if(!out.good()){
			std::cerr << "Failed to write a packet header to tga file\n";
			return false;
		}

		//Write the packet body
		out.write(reinterpret_cast<const char *>(data.data()+chunk_start), (raw?run_length*bytesPerPixel:bytesPerPixel));
		//Raw pixels should copy each pixel data consecutively, run length packets copy a single pixel that is copied the number of times in the packet header
		if(!out.good()){
			std::cerr << "Failed to write packet body to tga file\n";
			return false;
		}

	}

	return true;

}



//Mirrors pixels over the vertical axis
void TGAImage::flip_horizontally(){
	if(!data.size()) return;
	int half = width >> 1;

	for(int i=0; i<half; i++){
		for(int j=0; j<height; j++){
			TGAColor c1  = get(i, j);
			TGAColor c2 = get(width-i-1, j);
			set(i,j, c2);
			set(width-i-1, j, c1);
		}
	}
}

//Mirrors pixels over the horizontal axis
void TGAImage::flip_vertically(){
	if(!data.size()) return;
	size_t bytes_per_line = width*bytesPerPixel; //Flip entire rows
	std::vector<std::uint8_t> pixelBuffer(bytes_per_line, 0);
	int half = height >> 1;
	for(int j=0; j<half; j++){
		size_t l1 = j*bytes_per_line; //Get offset of first line
		size_t l2 = (height-j-1)*bytes_per_line; //Get offset of second line

		//Perform data swap using pixel buffer as temporary hold
		std::copy(data.begin()+l1, data.begin()+l1+bytes_per_line, pixelBuffer.begin());
		std::copy(data.begin()+l2, data.begin()+l2+bytes_per_line, data.begin()+l1);
		std::copy(pixelBuffer.begin(), pixelBuffer.end(), data.begin()+l2);
	}

}


//Scale the TGA Image
void TGAImage::scale(const int w, const int h){
	if(w<=0 || h<=0 || !data.size()) return;
	std::vector<std::uint8_t>scaledData(w*h*bytesPerPixel, 0);

	int scaledScanline = 0; 						//Current scan line of the scaled image
	int baseScanline = 0;							//Current scan line of the original image
	size_t scaledLineBytes = w*bytesPerPixel;	 	//Bytes in per line in the scaled image
	size_t baseLineBytes = width*bytesPerPixel;		//Bytes in per line in the original image

	int erry=0;										//Control for copying rows according to the ratio of height


	for(int j=0; j<height; j++){
		int errx = width-w;							//Control for copying the pixels according to ratio of widths
		int scaledX = -bytesPerPixel;				//x-pos of pixel in current row of scaled image
		int baseX = -bytesPerPixel; 				//x-pos of pixel in current row of original image

		for(int i=0; i<width; i++){
			baseX += bytesPerPixel;					//Iterate to next pixel
			errx += w;								
													//If width > w, this will copy a pixel over less than once per pixel of the original image (downsampling)
													//If width < w, this will copy a pixel over more than once per pixel of the original image (upsampling)
													//If width == w, every pixel will get copied over exactly once
			while(errx >= width){					
				errx -= width;
				scaledX += bytesPerPixel;
				memcpy(scaledData.data()+scaledScanline+scaledX, data.data()+baseScanline+baseX, bytesPerPixel);
			}

		}


		erry += h;
		baseScanline += baseLineBytes;				//Iterate to next row of original image

													//If height > h, the scaled image row variable "scaledScanline" will increment infrequently, the if statement will never evaluate to true
													//The while loop above will overwrite data frequently producing a image of less height (downsampling)
													//If height < h, occasionally "erry" will become twice as large as height, and a row will be duplicated (upsampling)
													//If height == h, the below while loop will always complete one iteration, but the if statement will never evaluate to true

		while(erry >= height){
			if(erry >= height << 1)					//Used to duplicate row n onto n+1 (upsampling height)
				memcpy(scaledData.data()+scaledScanline+scaledLineBytes, scaledData.data()+scaledScanline, scaledLineBytes);
			erry -=height;
			scaledScanline += scaledLineBytes; 		//Iterate to next row of scaled image


		}


	}

	//Update data to reflect the scaled image
	data = scaledData;
	width = w;
	height = h;

}


//Get pixel color
TGAColor TGAImage::get(const int x, const int y) const{

	if(!data.size() || x < 0 || x >= width || y<0 || y>=height) return {};
	return TGAColor(data.data()+(x+y*width)*bytesPerPixel, bytesPerPixel);

}	

//Set pixel color
void TGAImage::set(const int x, const int y, const TGAColor &c){
	if(!data.size() || x<0 || x>=width || y<0 || y>=height) return;
	memcpy(data.data()+(x+y*width)*bytesPerPixel, c.bgra, bytesPerPixel);
}

//Get TGA Image width
int TGAImage::get_width() const{
	return width;
}

//Get TGA Image height
int TGAImage::get_height() const{
	return height;
}

//Get TGA Image bytes per pixel
int TGAImage::get_bytespp(){
	return bytesPerPixel;
}

//Get the TGA Image data buffer
std::uint8_t* TGAImage::buffer(){
	return data.data();
}

//Clear the data buffer
void TGAImage::clear(){
	data = std::vector<std::uint8_t>(width*height*bytesPerPixel, 0);
}

//Debugging Information
void TGAImage::getInfo(){
	std::cerr << "TGA size: " << width <<"x" << height << "\n";
	std::cerr << "Pixels storage: " << bytesPerPixel << " bytes/pixel\n";
}
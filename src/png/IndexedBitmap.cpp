#include "IndexedBitmap.h"
#include <stdexcept>
#include <algorithm>
#include <Arduino.h> // Voor de ESP32, om Serial te gebruiken voor foutmeldingen

// --- Geheugenberekening ---

size_t IndexedBitmap::calculateTotalBytes() const {
    size_t totalPixels = static_cast<size_t>(width_) * height_;
    //size_t totalBits = totalPixels * bitsPerPixel_;
    
    // Gebruik (totalBits + 7) / 8 voor afronding naar boven naar het dichtstbijzijnde byte
    return (totalPixels);
}

// --- Constructor & Destructor ---

IndexedBitmap::IndexedBitmap(int width, int height, uint8_t bpp, uint8_t defaultColorIndex)
    : width_(width), height_(height), bitsPerPixel_(bpp) {
    
    // Valideer BPP
    if (bpp != 1 && bpp != 2 && bpp != 4 && bpp != 8) {
        throw std::invalid_argument("Bits per pixel moet 1, 2, 4 of 8 zijn.");
    }
    
    size_t totalBytes = calculateTotalBytes();
    
    // Alloceer geheugen (gebruik de standaard heap allocatie, kan ps_malloc zijn indien aangepast)
    data_ = new uint8_t[totalBytes]; 
//    Serial.printf("Bitmap size: %d\n", totalBytes );
//    Serial.printf("data at: %d\n", data_ );

    // Vul de bitmap met de standaard kleurindex
    clear(defaultColorIndex);
}

IndexedBitmap::~IndexedBitmap() {
    if (data_ != nullptr) {
        delete[] data_;
        data_ = nullptr;
    }
}

// --- Copy Constructors & Assignment Operator ---

IndexedBitmap::IndexedBitmap(const IndexedBitmap& other)
    : width_(other.width_), height_(other.height_), bitsPerPixel_(other.bitsPerPixel_) {

    Serial.println("====> copy constructor.");

    size_t totalBytes = calculateTotalBytes();
    data_ = new uint8_t[totalBytes];
    memcpy(data_, other.data_, totalBytes);
}

IndexedBitmap& IndexedBitmap::operator=(const IndexedBitmap& other) {
    if (this != &other) {

        Serial.println("====> assignment constructor.");

        // Geef oude geheugen vrij
        delete[] data_; 
        
        // Kopieer nieuwe dimensies en BPP
        width_ = other.width_;
        height_ = other.height_;
        bitsPerPixel_ = other.bitsPerPixel_;
        size_t totalBytes = calculateTotalBytes();
        
        // Alloceer nieuw geheugen en kopieer data
        data_ = new uint8_t[totalBytes];
        memcpy(data_, other.data_, totalBytes);
    }
    return *this;
}

// --- Publieke setPixel Functie ---

void IndexedBitmap::setPixel(int x, int y, uint8_t colorIndex) {
    if (x < 0 || x >= width_ || y < 0 || y >= height_) return;
        // Simpel 8 BPP geval
    data_[static_cast<size_t>(y) * width_ + x] = colorIndex;
    }

// --- De Aangepaste Clear Functie ---

void IndexedBitmap::clear(uint8_t colorIndex) {
    // Als de kleurindex 0 is, kunnen we de hele buffer snel met memset wissen.
    if (colorIndex == 0) {
        size_t totalBytes = calculateTotalBytes();
        memset(data_, 0, totalBytes);
    } else {
        // Als de kleurindex niet 0 is, moeten we elke pixel apart instellen
        // om de bit-packing correct uit te voeren.
        for (int y = 0; y < height_; ++y) {
            for (int x = 0; x < width_; ++x) {
                setPixel(x, y, colorIndex);
            }
        }
    }
}

// --- Overige Functies ---

uint8_t* IndexedBitmap::getData() const {
    return data_;
}

void IndexedBitmap::copyTo(IndexedBitmap& dest, int srcX, int srcY, int destX, int destY, int w, int h) const {
    // WAARSCHUWING: Dit werkt alleen betrouwbaar als beide BPP's Gelijk zijn!
    if (bitsPerPixel_ != dest.bitsPerPixel_) {
        // Je zou hier een conversielogica moeten toevoegen voor echte productiecode.
        Serial.println("Fout: copyTo alleen ondersteund voor bitmaps met gelijke BPP.");
        return; 
    }

    int clipW = std::min({w, width_ - srcX, dest.width_ - destX});
    int clipH = std::min({h, height_ - srcY, dest.height_ - destY});
    
    if (clipW <= 0 || clipH <= 0) return;
    
    // Gezien de complexiteit van BPP packing, kopiëren we rij voor rij en pixel voor pixel (veiliger dan memcpy)
    // Dit is trager dan een enkele memcpy, maar veilig voor BPP > 1
    for (int y = 0; y < clipH; ++y) {
        for (int x = 0; x < clipW; ++x) {
            // Dit is een simpele implementatie:
            // Voor snellere code zou men moeten controleren of de rij in de bron en bestemming
            // *perfect* op een byte-grens liggen en dan pas memcpy gebruiken.
            
            // Haal de kleurindex op (omgekeerde van setPackedPixel)
            // Nu gebruiken we setPixel voor de bestemming
            // LET OP: Hiervoor is een getPixel functie nodig, 
            // of we gebruiken setPackedPixel/setPixel direct als we de index al kennen.
            
            // Voor nu, laten we dit versimpelen en alleen 8 BPP ondersteunen voor memcpy.

                size_t srcOffset = static_cast<size_t>(srcY + y) * width_ + srcX;
                size_t destOffset = static_cast<size_t>(destY + y) * dest.width_ + destX;
                memcpy(dest.data_ + destOffset, data_ + srcOffset, clipW);

        }
    }
}


void IndexedBitmap::cutAndPaste(IndexedBitmap& dest, int srcX, int srcY, int destX, int destY, int w, int h, uint8_t clearColor) {
    // 1. Kopieer het gebied
    copyTo(dest, srcX, srcY, destX, destY, w, h);
    
    // 2. Wis het brongebied in de huidige bitmap (de "cut" stap)
    // Dit kan geoptimaliseerd worden, maar met setPixel/clear is het veilig.
    for (int y = 0; y < h; ++y) {
         for (int x = 0; x < w; ++x) {
            setPixel(srcX + x, srcY + y, clearColor);
        }
    }
}
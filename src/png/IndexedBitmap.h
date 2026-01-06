#ifndef INDEXED_BITMAP_H
#define INDEXED_BITMAP_H

#include <cstdint>   // Voor uint8_t en size_t
#include <cstring>   // Voor memcpy en memset
#include <algorithm> // Voor std::min en std::max
#include <stdexcept> // Voor uitzonderingsafhandeling

/**
 * @class IndexedBitmap
 * @brief Beheert een bitmap met geindexeerde kleuren (1, 2, 4, of 8 bits per pixel).
 */
class IndexedBitmap {
public:
    // --- Constructor & Destructor ---

    /**
     * @brief Constructor: Maakt een bitmap en alloceert heap-geheugen.
     * @param width Breedte van de bitmap in pixels.
     * @param height Hoogte van de bitmap in pixels.
     * @param bpp Bits per pixel (1, 2, 4 of 8).
     * @param defaultColorIndex De kleurindex om de bitmap mee te vullen (standaard 0).
     */
    IndexedBitmap(int width, int height, uint8_t bpp, uint8_t defaultColorIndex = 0);

    /**
     * @brief Destructor: Geeft het gealloceerde geheugen vrij.
     */
    ~IndexedBitmap();

    // Copy Constructor & Assignment Operator voor deep copy
    IndexedBitmap(const IndexedBitmap& other);
    IndexedBitmap& operator=(const IndexedBitmap& other);

    // copy only the bitmap without new/delete
    void CopyFromBitmap( const IndexedBitmap& other) ;

    // --- Core Functies ---

    /**
     * @brief Stelt de kleurindex van een individuele pixel in.
     */
    void setPixel(int x, int y, uint8_t colorIndex);
    void drawLine(int x0, int y0, int x1, int y1, uint8_t colorIndex);


    /**
     * @brief Maakt de hele bitmap leeg met een specifieke kleurindex.
     */
    void clear(uint8_t colorIndex = 0);

    /**
     * @brief Geeft een pointer naar de ruwe pixel data terug.
     */
    uint8_t* getData() const;

    // --- Geometrie Functies ---

    /**
     * @brief Kopieert een rechthoekig gebied van deze bitmap naar een andere bitmap.
     */
    void copyTo(IndexedBitmap& dest, int srcX, int srcY, int destX, int destY, int w, int h) const;

    /**
     * @brief Kopieert een rechthoekig gebied naar een bestemming en wist het brongebied ("cut").
     */
    void cutAndPaste(IndexedBitmap& dest, int srcX, int srcY, int destX, int destY, int w, int h, uint8_t clearColor = 0);

    // --- Getters ---
    int getWidth() const { return width_; }
    int getHeight() const { return height_; }
    uint8_t getBPP() const { return bitsPerPixel_; }


private:
    int width_;
    int height_;
    uint8_t bitsPerPixel_;
    uint8_t* data_ = nullptr; // De ruwe, ingepakte pixel data

    /**
     * @brief Berekent het totale aantal bytes nodig voor de bitmap.
     */
    size_t calculateTotalBytes() const;

    /**
     * @brief Interne helper om een enkele pixel in de ingepakte byte array te zetten.
     */
    void setPackedPixel(int x, int y, uint8_t colorIndex);
};

#endif // INDEXED_BITMAP_H
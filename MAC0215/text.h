#ifndef TEXT_H
#define TEXT_H

#include <QString>
#include <QDebug>
#include <QVector3D>
#include <QVector2D>
#include <vector>
#include <cmath>
#include <QImage>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "glyph.h"
#include "pdf_extractor.h"

class text {
public:
    text(QString font);

    void define_font_type (QString font);
    void define_text (QString t, std::vector<QVector3D> quad_vertices);
    void bake_atlas ();
    void bake_mip_atlas (int max_resolution, int max_size, int layers, QString image_path);
    void define_text_from_pdf (QString pdf_path);
    void gen_test ();
    void gen_test_pdf ();

    std::vector<QVector3D> font_vertices;
    std::vector<QVector2D> font_texture;

private:
    QString font_path;
    QString text_to_render;
    FT_Library ft;
    FT_Face face;

    std::vector<std::vector<glyph>> glyph_set;
};

#endif // TEXT_H

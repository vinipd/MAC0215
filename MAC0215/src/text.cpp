#include "text.h"
#include <iostream>
#include <locale>
#include <QDebug>

int dpi_test = 500;
int resol_test = 1000;

// Constructor
text::text (QString font, QString atlas) {
    define_font_type (QString (font));
    define_atlas (atlas);
    set_layer (1);
    /*
    image test;
    bake_type = "distance transform city_block";

    for (int i = 0; i < 5; ++i) {
        image_line line;
        for (int j = 0; j < 5; ++j)
            line.push_back(1);
        test.push_back (line);
    }

    test[4][4] = 0;

    distance_transform transform_test (test, 5, 5);
    do_transform (transform_test);

    for (int i = 0; i < 5; ++i)
        for (int j = 0; j < 5; ++j)
        qDebug () << transform_test.get_transform_element(i,j);

    */

}

/*** Public metthods ***/

void text::bake_atlas() {
    bake_type = "trivial";
    bake (dpi_test, resol_test);
}

void text::bake_dist_transf (QString metric) {
    bake_type = "distance transform " + metric;
    bake (dpi_test, resol_test);
}

void text::bake_mip_atlas (int max_resolution, int max_size, int layers) {
    set_layer (layers);
    bake_type = "mip map";
    bake (max_resolution, max_size);
}

void text::define_text (QString t, std::vector<QVector3D> quad_vertices) {
    text_to_render = QString (t);

    set_layer (0);

    font_vertices.clear();
    font_texture.clear();

    int i = 0;
    for (QChar *c = text_to_render.begin(); c != text_to_render.end(); ++c, i += 4) {
        font_vertices.push_back(quad_vertices[i]);
        font_vertices.push_back(quad_vertices[i + 1]);
        font_vertices.push_back(quad_vertices[i + 2]);
        font_vertices.push_back(quad_vertices[i + 2]);
        font_vertices.push_back(quad_vertices[i + 1]);
        font_vertices.push_back(quad_vertices[i + 3]);

        glyph g = glyph_set[layer][c->unicode()];

        float x_offset = (1.0 * g.get_x_offset()) / x_size;
        float y_offset = (1.0 * g.get_y_offset()) / y_size;
        float height = (1.0 * g.get_height ()) / y_size;
        float width = (1.0 * g.get_width ()) / x_size;

        font_texture.push_back(QVector2D(x_offset, 1 - (y_offset + height)));
        font_texture.push_back(QVector2D(x_offset + width, 1 - (y_offset + height)));
        font_texture.push_back(QVector2D(x_offset, 1 - y_offset));
        font_texture.push_back(QVector2D(x_offset, 1 - y_offset));
        font_texture.push_back(QVector2D(x_offset + width, 1 - (y_offset + height)));
        font_texture.push_back(QVector2D(x_offset + width, 1 - y_offset));
    }
}

void text::define_text_from_pdf (QString pdf_path) {
    pdf_extractor *extractor = new pdf_extractor (pdf_path.toStdString(), code_path.toStdString());
    QString txt;
    double *bbox;
    int num_pages;

    extractor->init ();
    num_pages = extractor->extract ();
    txt = QString (extractor->get_text(0).c_str());
    bbox = extractor->get_bbox(0);
    extractor->end ();

    QString highlowers = "bdfhijklt";
    QString pontuation = ",.;";

    std::vector<QVector3D> txt_vertices;
    int i, j;
    for (i = 0, j = 0; i < txt.size(); ++i, j += 4) {
        float scale = 0.5;
        float low_y_scale = 1;
        float low_x_scale = 1;
        if (txt[i].isLower() && !highlowers.contains(txt[i], Qt::CaseSensitive))
            low_y_scale = 0.7;
        if (pontuation.contains(txt[i]))
            low_y_scale = 0.3;
        txt_vertices.push_back (QVector3D (bbox[j], bbox[j + 1], 0)*scale);
        txt_vertices.push_back (QVector3D (bbox[j] +  (bbox[j + 2] - bbox[j]) * low_x_scale, bbox[j + 1], 0)*scale);
        txt_vertices.push_back (QVector3D (bbox[j], bbox[j + 1] + (bbox[j + 3] - bbox[j + 1]) * low_y_scale, 0)*scale);
        txt_vertices.push_back (QVector3D (bbox[j] +  (bbox[j + 2] - bbox[j]) * low_x_scale, bbox[j + 1] + (bbox[j + 3] - bbox[j + 1]) * low_y_scale, 0)*scale);
    }

    define_text (txt, txt_vertices);
}

void text::set_layer (int l) {
    layer = l;
}

void text::gen_test () {
    QString test_string ("Olá!");

    std::vector<QVector3D> vec;
    vec.push_back(QVector3D (-2.0, 0, 0));
    vec.push_back(QVector3D (-1.5, 0, 0));
    vec.push_back(QVector3D (-2.0, 0.5, 0));
    vec.push_back(QVector3D (-1.5, 0.5, 0));

    vec.push_back(QVector3D (-1.5, 0, 0));
    vec.push_back(QVector3D (-1.0, 0, 0));
    vec.push_back(QVector3D (-1.5, 0.5, 0));
    vec.push_back(QVector3D (-1.0, 0.5, 0));

    vec.push_back(QVector3D (-1.0, 0, 0));
    vec.push_back(QVector3D (-0.5, 0, 0));
    vec.push_back(QVector3D (-1.0, 0.5, 0));
    vec.push_back(QVector3D (-0.5, 0.5, 0));

    vec.push_back(QVector3D (-0.5, 0, 0));
    vec.push_back(QVector3D (-0.3, 0, 0));
    vec.push_back(QVector3D (-0.5, 0.5, 0));
    vec.push_back(QVector3D (-0.3, 0.5, 0));

    define_text(test_string, vec);
}

void text::gen_test_pdf () {
    QString example_path = pdf_test_path + "Lorem-Ipsum.pdf";
    define_text_from_pdf (example_path);
}

/*** Private methods ***/

void text::bake (int max_resolution, int max_size) {
    if (max_resolution > 500 || max_size > 1000) {
        qDebug () << "Too high!\n";
        return;
    }
    if (FT_Init_FreeType (&ft))
        qDebug() << "ruim init";
    if(FT_New_Face(ft, font_path.toStdString().c_str(), 0, &face))
        qDebug() << "ruim new face";

    int GLYPH_HEIGHT = max_size;
    int DPI = atlas_resolution * max_resolution;
    int padding = 5;

    std::locale loc("en_US.UTF-8");
    /*if (bake_type.indexOf("distance transform") > -1)
        FT_Set_Char_Size (face, 0, GLYPH_HEIGHT, DPI/2, DPI/2);
    else*/
    FT_Set_Char_Size (face, 0, GLYPH_HEIGHT, DPI, DPI);
    int num_glyphs = face->num_glyphs;

    FT_Size_Metrics_ metric = face->size->metrics;
    uint texture_height = (metric.height >> 6) * (sqrt(num_glyphs));
    uint texture_width = texture_height;
    x_size = texture_width;
    y_size = texture_height;

    if (layer > 1)
        x_size *= 1.5;

    int x, y;
    int last_x = 0, last_y = 0;

    glyph_set.clear();
    QImage texture (texture_width, texture_height, QImage::Format_RGB32);
    for (int l = 0; l < 1; ++l) {
        std::vector<glyph> set;
        x = last_x, y = last_y;
        for (int i = 0; i < num_glyphs; ++i) {
            FT_Load_Char (face, i, FT_LOAD_RENDER | FT_LOAD_TARGET_LIGHT);
            FT_Bitmap *bmp = &face->glyph->bitmap;
            if (std::isprint((wchar_t) i, loc)) {
                image img;

                if (x + bmp->width >= texture_width)
                    x = 0, y += (padding + (face->size->metrics.height >> 6));

                img = construct_image (bmp);
                distance_transform transform (img, bmp->rows, bmp->width);
                do_transform (transform);

                prepare_texture (transform, texture, x, y);
                set.push_back (glyph (x, y, bmp->rows, bmp->width, i));

                x +=  padding + bmp->width;
            }
            else
                set.push_back (glyph (0, 0, 0, 0, i));
        }
        glyph_set.push_back(set);
        if ((l+1) % 2)
            last_x += texture_width;
        else
            last_y += texture_height;
        texture_width = texture_width >> 1;
        texture_height = texture_height >> 1;
        DPI = DPI >> 1;
        FT_Set_Char_Size (face, 0, GLYPH_HEIGHT, DPI, DPI);
    }

    QImage test = texture.scaled(atlas_dimension * texture.height(), atlas_dimension * texture.width(),Qt::KeepAspectRatio);
    test.save(atlas_path, Q_NULLPTR, 50);
    FT_Done_FreeType(ft);
}

void text::do_transform (distance_transform &transform) {
    if (bake_type == "distance transform city_block")
        transform.city_block ();
    else if (bake_type == "distance transform chessboard")
        transform.chess_board ();
    else if (bake_type == "distance transform euclidean")
        transform.faster_euclidean ();
    else if (bake_type == "distance transform fast marching")
        transform.fast_marching ();

}

image text::construct_image (FT_Bitmap *bmp) {
    image img;
    for (uint row = 0; row < bmp->rows; ++row) {
        image_line line;
        for (uint col = 0; col < bmp->width; ++col) {
            if (bmp->buffer[row * bmp->pitch + col])
                line.push_back(1);
            else line.push_back(0);
        }
        img.push_back(line);
    }

    return img;
}

void text::prepare_texture (distance_transform transform, QImage &texture, int x_off, int y_off) {
    QRgb color;

    float max = transform.get_height ();
    if (transform.get_metric() == "euclidean")
        max = sqrt (transform.get_width()*transform.get_width() + transform.get_height ()*transform.get_width());
    if (transform.get_metric() == "city_block")
        max = transform.get_height () +  transform.get_width ();
    if (transform.get_metric() == "chessboard")
        max = transform.get_height () + transform.get_width();
    if (transform.get_metric() == "fast marching")
        max = sqrt(transform.get_height ()*transform.get_height () + transform.get_width()*transform.get_width());
    else if (transform.get_width () > max) max = transform.get_width ();


    for (int row = 0; row < transform.get_height (); ++row) {
        int color_cmp;
        float element;
        for (int col = 0; col < transform.get_width (); ++col) {
            element = transform.get_transform_element (row, col) / max;
            if (transform.get_metric() == "trivial")
                color_cmp = 255 * transform.get_transform_element (row, col);
            else
                color_cmp = 255 * (0.5 * element + 0.5);
            if (color_cmp > 255) color_cmp = 255;
            if (color_cmp < 0) color_cmp = 0;
            color = qRgb (color_cmp, color_cmp, color_cmp);
            texture.setPixel (col + x_off, row + y_off, color);
        }
    }
}

void text::define_font_type (QString font) {
    font_path = font;
}

void text::define_atlas (QString atlas) {
    atlas_path = atlas;
}

void text::set_atlas_dimension_value (float new_value) {
    atlas_dimension = new_value;
}

void text::set_atlas_resolution_value (float new_value) {
    atlas_resolution = new_value;
}

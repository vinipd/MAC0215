#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QString>
#include <QKeyEvent>
#include <QMatrix4x4>
#include <QMainWindow>
#include <QOpenGLWidget>
#include <QOpenGLWindow>
#include <QOpenGLBuffer>
#include <QOpenGLShader>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLTexture>

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)

#include "camera3d.h"
#include "transform3d.h"
#include "text.h"

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    GLWidget(int step, QWidget *parent = 0);
    ~GLWidget();

    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;

    void connectUpdate();
    void disconnectUpdate();
    void LoadText (int layers);
    void set_render_mode (int layers);
    void set_bake_type (QString new_bake_type);
    void set_transform_type (QString new_transform_type);
    void set_trivial_type (QString new_trivial_type);
    void set_gamma_value (float new_value);
    void set_atlas_dimension_value (float new_value);

    float albedo, last_albedo;
    bool changed = false;
    bool change_render = false;
    text *Text;

    QOpenGLShaderProgram *m_program;
    QOpenGLShader *shader;

public slots:
    void cleanup();

protected:
    void paintGL() override;
    void initializeGL() override;
    void resizeGL(int width, int height) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;


protected slots:
  void update();

private:
    void printContextInformation();
    void initTex (QString url);
    void loadTexture (QString file);

    QString transform_type = "";
    QString trivial_type = "";
    QString bake_type = "";

    QOpenGLBuffer m_vertex;
    QOpenGLBuffer m_normal;
    QOpenGLBuffer m_tex;

    QOpenGLVertexArrayObject m_object;

    // Shader Information
    int u_modelToWorld;
    int u_cameraToView;
    int u_worldToCamera;
    int u_gamma;

    Camera3D m_camera;
    QMatrix4x4 m_projection;
    Transform3D m_transform;

    int step_;
    int indiceSize_;
    int vertexCount_;
    int m_texAttr;
    GLuint* indices_;
    QVector3D* sg_vertexes_ = NULL;
    QVector3D *vertices;
    QVector2D *Texture, *sg_texture_ = NULL;
    GLuint *indices;

    float dist_to_pages = -100.0;
    float zoom_factor = 0.65;
    float w_size, h_size;

    float gamma_value = 0.05;
    float atlas_dimension_value = 0.5;

    QMetaObject::Connection m_connection;

};

#endif

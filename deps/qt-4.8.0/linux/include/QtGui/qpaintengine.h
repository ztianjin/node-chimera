/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QPAINTENGINE_H
#define QPAINTENGINE_H

#include <QtCore/qnamespace.h>
#include <QtCore/qobjectdefs.h>
#include <QtCore/qscopedpointer.h>
#include <QtGui/qpainter.h>
#include <QtCore/qurl.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

class QFontEngine;
class QLineF;
class QPaintDevice;
class QPaintEnginePrivate;
class QPainterPath;
class QPointF;
class QPolygonF;
class QRectF;
struct QGlyphLayout;
class QTextItemInt;
class QPaintEngineState;

class Q_GUI_EXPORT QTextItem {
public:
    enum RenderFlag {
        RightToLeft = 0x1,
        Overline = 0x10,
        Underline = 0x20,
        StrikeOut = 0x40,

        Dummy = 0xffffffff
    };
    Q_DECLARE_FLAGS(RenderFlags, RenderFlag)
    qreal descent() const;
    qreal ascent() const;
    qreal width() const;

    RenderFlags renderFlags() const;
    QString text() const;
    QFont font() const;
};
Q_DECLARE_TYPEINFO(QTextItem, Q_PRIMITIVE_TYPE);


class Q_GUI_EXPORT QPaintEngine
{
    Q_DECLARE_PRIVATE(QPaintEngine)
public:
    enum PaintEngineFeature {
        PrimitiveTransform          = 0x00000001, // Can transform primitives brushes
        PatternTransform            = 0x00000002, // Can transform pattern brushes
        PixmapTransform             = 0x00000004, // Can transform pixmaps
        PatternBrush                = 0x00000008, // Can fill with pixmaps and standard patterns
        LinearGradientFill          = 0x00000010, // Can fill gradient areas
        RadialGradientFill          = 0x00000020, // Can render radial gradients
        ConicalGradientFill         = 0x00000040, // Can render conical gradients
        AlphaBlend                  = 0x00000080, // Can do source over alpha blend
        PorterDuff                  = 0x00000100, // Can do general porter duff compositions
        PainterPaths                = 0x00000200, // Can fill, outline and clip paths
        Antialiasing                = 0x00000400, // Can antialias lines
        BrushStroke                 = 0x00000800, // Can render brush based pens
        ConstantOpacity             = 0x00001000, // Can render at constant opacity
        MaskedBrush                 = 0x00002000, // Can fill with textures that has an alpha channel or mask
        PerspectiveTransform        = 0x00004000, // Can do perspective transformations
        BlendModes                  = 0x00008000, // Can do extended Porter&Duff composition
        ObjectBoundingModeGradients = 0x00010000, // Can do object bounding mode gradients
        RasterOpModes               = 0x00020000, // Can do logical raster operations
        PaintOutsidePaintEvent      = 0x20000000, // Engine is capable of painting outside paint events
        /*                          0x10000000, // Used for emulating
                                    QGradient::StretchToDevice,
                                    defined in qpainter.cpp

                                    0x40000000, // Used internally for emulating opaque backgrounds
        */

        AllFeatures               = 0xffffffff  // For convenience
    };
    Q_DECLARE_FLAGS(PaintEngineFeatures, PaintEngineFeature)

    enum DirtyFlag {
        DirtyPen                = 0x0001,
        DirtyBrush              = 0x0002,
        DirtyBrushOrigin        = 0x0004,
        DirtyFont               = 0x0008,
        DirtyBackground         = 0x0010,
        DirtyBackgroundMode     = 0x0020,
        DirtyTransform          = 0x0040,
        DirtyClipRegion         = 0x0080,
        DirtyClipPath           = 0x0100,
        DirtyHints              = 0x0200,
        DirtyCompositionMode    = 0x0400,
        DirtyClipEnabled        = 0x0800,
        DirtyOpacity            = 0x1000,

        AllDirty                = 0xffff
    };
    Q_DECLARE_FLAGS(DirtyFlags, DirtyFlag)

    enum PolygonDrawMode {
        OddEvenMode,
        WindingMode,
        ConvexMode,
        PolylineMode
    };

    explicit QPaintEngine(PaintEngineFeatures features=0);
    virtual ~QPaintEngine();

    bool isActive() const { return active; }
    void setActive(bool newState) { active = newState; }

    virtual bool begin(QPaintDevice *pdev) = 0;
    virtual bool end() = 0;

    virtual void updateState(const QPaintEngineState &state) = 0;

    virtual void drawRects(const QRect *rects, int rectCount);
    virtual void drawRects(const QRectF *rects, int rectCount);

    virtual void addHyperlink(const QRectF &r, const QUrl &url) {Q_UNUSED(r); Q_UNUSED(url);}
    virtual void addAnchor(const QRectF &r, const QString &name) {Q_UNUSED(r); Q_UNUSED(name);}
    virtual void addLink(const QRectF &r, const QString &anchor) {Q_UNUSED(r); Q_UNUSED(anchor);}
    virtual void addTextField(const QRectF &r, const QString &text, const QString &name, bool multiLine, bool password, bool readOnly, int maxLength) {
        Q_UNUSED(r); Q_UNUSED(text); Q_UNUSED(name); Q_UNUSED(multiLine); Q_UNUSED(password); Q_UNUSED(readOnly); Q_UNUSED(maxLength);
    }
    virtual void addCheckBox(const QRectF &r, bool checked, const QString &name, bool readOnly) {
        Q_UNUSED(r); Q_UNUSED(checked); Q_UNUSED(name); Q_UNUSED(readOnly);
    }
    virtual void addRadioButton(const QRectF &r, const QString & group="", bool checked=false, const QString &name="", bool readOnly=false) {
        Q_UNUSED(r); Q_UNUSED(checked); Q_UNUSED(name); Q_UNUSED(readOnly); Q_UNUSED(group);
    }

    virtual void drawLines(const QLine *lines, int lineCount);
    virtual void drawLines(const QLineF *lines, int lineCount);

    virtual void drawEllipse(const QRectF &r);
    virtual void drawEllipse(const QRect &r);

    virtual void drawPath(const QPainterPath &path);

    virtual void drawPoints(const QPointF *points, int pointCount);
    virtual void drawPoints(const QPoint *points, int pointCount);

    virtual void drawPolygon(const QPointF *points, int pointCount, PolygonDrawMode mode);
    virtual void drawPolygon(const QPoint *points, int pointCount, PolygonDrawMode mode);

    virtual void drawPixmap(const QRectF &r, const QPixmap &pm, const QRectF &sr) = 0;
    virtual void drawPixmap(const QRectF &r, const QPixmap &pm, const QRectF &sr, const QByteArray * data) {
        Q_UNUSED(data);
        drawPixmap(r,pm,sr);
    }

    virtual void drawTextItem(const QPointF &p, const QTextItem &textItem);
    virtual void drawTiledPixmap(const QRectF &r, const QPixmap &pixmap, const QPointF &s);
    virtual void drawImage(const QRectF &r, const QImage &pm, const QRectF &sr,
                           Qt::ImageConversionFlags flags = Qt::AutoColor);

    void setPaintDevice(QPaintDevice *device);
    QPaintDevice *paintDevice() const;

    void setSystemClip(const QRegion &baseClip);
    QRegion systemClip() const;

    void setSystemRect(const QRect &rect);
    QRect systemRect() const;

#ifdef Q_WS_WIN
    virtual HDC getDC() const;
    virtual void releaseDC(HDC hdc) const;
#endif

    virtual QPoint coordinateOffset() const;

    enum Type {
        X11,
        Windows,
        QuickDraw, CoreGraphics, MacPrinter,
        QWindowSystem,
        PostScript,
        OpenGL,
        Picture,
        SVG,
        Raster,
        Direct3D,
        Pdf,
        OpenVG,
        OpenGL2,
        PaintBuffer,
        Blitter,

        User = 50,    // first user type id
        MaxUser = 100 // last user type id
    };
    virtual Type type() const = 0;

    inline void fix_neg_rect(int *x, int *y, int *w, int *h);

    inline bool testDirty(DirtyFlags df);
    inline void setDirty(DirtyFlags df);
    inline void clearDirty(DirtyFlags df);

    bool hasFeature(PaintEngineFeatures feature) const { return (gccaps & feature) != 0; }

    QPainter *painter() const;

    void syncState();
    inline bool isExtended() const { return extended; }

protected:
    QPaintEngine(QPaintEnginePrivate &data, PaintEngineFeatures devcaps=0);

    QPaintEngineState *state;
    PaintEngineFeatures gccaps;

    uint active : 1;
    uint selfDestruct : 1;
    uint extended : 1;

    QScopedPointer<QPaintEnginePrivate> d_ptr;

private:
    void setAutoDestruct(bool autoDestr) { selfDestruct = autoDestr; }
    bool autoDestruct() const { return selfDestruct; }
    Q_DISABLE_COPY(QPaintEngine)

    friend class QPainterReplayer;
    friend class QFontEngineBox;
    friend class QFontEngineMac;
    friend class QFontEngineWin;
#ifndef QT_NO_FREETYPE
    friend class QFontEngineFT;
#endif
#ifndef QT_NO_QWS_QPF
    friend class QFontEngineQPF1;
#endif
#ifndef QT_NO_QWS_QPF2
    friend class QFontEngineQPF;
#endif
    friend class QPSPrintEngine;
    friend class QMacPrintEngine;
    friend class QMacPrintEnginePrivate;
#ifdef Q_WS_QWS
    friend class QtopiaPrintEngine;
    friend class QtopiaPrintEnginePrivate;
    friend class QProxyFontEngine;
#endif
#ifdef Q_WS_QPA
    friend class QFontEngineQPA;
#endif
    friend class QPainter;
    friend class QPainterPrivate;
    friend class QWidget;
    friend class QWidgetPrivate;
    friend class QWin32PaintEngine;
    friend class QWin32PaintEnginePrivate;
    friend class QMacCGContext;
    friend class QPreviewPaintEngine;
    friend class QX11GLPixmapData;
};


class Q_GUI_EXPORT QPaintEngineState
{
public:
    QPaintEngine::DirtyFlags state() const { return dirtyFlags; }

    QPen pen() const;
    QBrush brush() const;
    QPointF brushOrigin() const;
    QBrush backgroundBrush() const;
    Qt::BGMode backgroundMode() const;
    QFont font() const;
    QMatrix matrix() const;
    QTransform transform() const;

    Qt::ClipOperation clipOperation() const;
    QRegion clipRegion() const;
    QPainterPath clipPath() const;
    bool isClipEnabled() const;

    QPainter::RenderHints renderHints() const;
    QPainter::CompositionMode compositionMode() const;
    qreal opacity() const;

    QPainter *painter() const;

    bool brushNeedsResolving() const;
    bool penNeedsResolving() const;

protected:
    friend class QPaintEngine;
    friend class QRasterPaintEngine;
    friend class QWidget;
    friend class QPainter;
    friend class QPainterPrivate;
    friend class QMacPrintEnginePrivate;

    QPaintEngine::DirtyFlags dirtyFlags;
};

//
// inline functions
//

inline void QPaintEngine::fix_neg_rect(int *x, int *y, int *w, int *h)
{
    if (*w < 0) {
        *w = -*w;
        *x -= *w - 1;
    }
    if (*h < 0) {
        *h = -*h;
        *y -= *h - 1;
    }
}

inline bool QPaintEngine::testDirty(DirtyFlags df) {
    Q_ASSERT(state);
    return ((state->dirtyFlags & df) != 0);
}

inline void QPaintEngine::setDirty(DirtyFlags df) {
    Q_ASSERT(state);
    state->dirtyFlags |= df;
}

inline void QPaintEngine::clearDirty(DirtyFlags df)
{
    Q_ASSERT(state);
    state->dirtyFlags &= ~static_cast<uint>(df);
}

Q_DECLARE_OPERATORS_FOR_FLAGS(QTextItem::RenderFlags)
Q_DECLARE_OPERATORS_FOR_FLAGS(QPaintEngine::PaintEngineFeatures)
Q_DECLARE_OPERATORS_FOR_FLAGS(QPaintEngine::DirtyFlags)

QT_END_NAMESPACE

QT_END_HEADER

#endif // QPAINTENGINE_H
/*
 * ZKDiagram.h
 *
 *  Created on: Jul 31, 2017
 *      Author: guoxs
 */

#ifndef _CONTROL_ZKDIAGRAM_H_
#define _CONTROL_ZKDIAGRAM_H_

#include <vector>
#include "ZKBase.h"

class ZKDiagramPrivate;

class ZKDiagram : public ZKBase {
	ZK_DECLARE_PRIVATE(ZKDiagram)

public:
	typedef enum {
		E_DIAGRAM_STYLE_LINE,		// 折线
		E_DIAGRAM_STYLE_CURVE		// 曲线
	} EDiagramStyle;

public:
	ZKDiagram(HWND hParentWnd);
	virtual ~ZKDiagram();

	void setPenWidth(int index, int width);
	void setPenColor(int index, ARGB color);
	void setXScale(int index, double xScale);
	void setYScale(int index, double yScale);
	void setData(int index, const MPPOINT *pPoints, int count);

	void addData(int index, float data);
	void addDiagramInfo(int width, ARGB color, EDiagramStyle style,
			double xScale, double yScale, float step, UINT eraseSpace, bool isAntialiasOn);

protected:
	ZKDiagram(HWND hParentWnd, ZKBasePrivate *pBP);

	virtual void onBeforeCreateWindow(const Json::Value &json);
	virtual const char* getClassName() const { return ZK_DIAGRAM; }

	virtual void onDraw(HDC hdc);

	void _section_(zk) drawDiagram(HDC hdc);

private:
	void _section_(zk) parseDiagramAttributeFromJson(const Json::Value &json);

private:
	typedef struct {
		float lower;
		float upper;
	} SAxisRange;

	typedef struct {
		HGRAPHICS graphics;
		HPATH path;
		HPEN pen;
		int penWidth;
		ARGB penColor;
		EDiagramStyle style;
		double xScale;
		double yScale;
		MPPOINT bufPoints[8];
		int bufIndex;
		float step;
		UINT eraseSpace;
		MPMatrix matrix;
	} SDiagramInfo;

	void updateMatrix(SDiagramInfo &info);
	void transform(const MPMatrix &matrix, float &x, float &y) const;

private:
	vector<SDiagramInfo *> mDiagramInfoList;

	LayoutPosition mRegionPosition;		// 曲线图绘制区域

	SAxisRange mXAxisRange;		// X轴坐标范围
	SAxisRange mYAxisRange;		// Y轴坐标范围

	double mXBaseScale;
	double mYBaseScale;
};

#endif /* _CONTROL_ZKDIAGRAM_H_ */

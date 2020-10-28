/*
 * ZKPainter.h
 *
 *  Created on: Feb 20, 2019
 *      Author: guoxs
 */

#ifndef _CONTROL_ZKPAINTER_H_
#define _CONTROL_ZKPAINTER_H_

#include "ZKBase.h"

class ZKPainterPrivate;

/**
 * @brief 画布控件
 */
class ZKPainter : public ZKBase {
	ZK_DECLARE_PRIVATE(ZKPainter)

public:
	ZKPainter(HWND hParentWnd);
	virtual ~ZKPainter();

	/**
	 * @brief 设置线宽
	 * @param width 线宽
	 */
	void setLineWidth(int width);

	/**
	 * @brief 设置颜色值
	 * @param color 颜色值为0x RGB
	 */
	void setSourceColor(int color);

	/**
	 * @brief 画三角形
	 * @param x0 第一个顶点X坐标
	 * @param y0 第一个顶点Y坐标
	 * @param x1 第二个顶点X坐标
	 * @param y1 第二个顶点Y坐标
	 * @param x2 第三个顶点X坐标
	 * @param y2 第三个顶点Y坐标
	 */
	void drawTriangle(int x0, int y0, int x1, int y1, int x2, int y2);

	/**
	 * @brief 画矩形
	 * @param left 左上角X坐标
	 * @param top 左上角Y坐标
	 * @param width 矩形宽度
	 * @param height 矩形高度
	 * @param radiusX X轴方向圆角半径，默认值为：0，为不带圆角矩形
	 * @param radiusY Y轴方向圆角半径，默认值为：0，为不带圆角矩形
	 */
	void drawRect(int left, int top, int width, int height, int radiusX = 0, int radiusY = 0);

	/**
	 * @brief 画圆弧
	 * @param centerX 中心点X坐标
	 * @param centerY 中心点Y坐标
	 * @param radiusX X轴方向半径
	 * @param radiusY Y轴方向半径，默认值为：0，表示X与Y方向半径相等，为圆形，否则为椭圆形
	 * @param startAngle 起始角度，垂直方向向上为0度方向，顺时针方向为正，逆时针方向为负； 默认值为：0度
	 * @param sweepAngle 偏移角度，顺时针方向为正，逆时针方向为负； 默认值为：360度
	 */
	void drawArc(int centerX, int centerY, int radiusX, int radiusY = 0, int startAngle = 0, int sweepAngle = 360);

	/**
	 * @brief 填充三角形
	 * @param x0 第一个顶点X坐标
	 * @param y0 第一个顶点Y坐标
	 * @param x1 第二个顶点X坐标
	 * @param y1 第二个顶点Y坐标
	 * @param x2 第三个顶点X坐标
	 * @param y2 第三个顶点Y坐标
	 */
	void fillTriangle(int x0, int y0, int x1, int y1, int x2, int y2);

	/**
	 * @brief 填充矩形
	 * @param left 左上角X坐标
	 * @param top 左上角Y坐标
	 * @param width 矩形宽度
	 * @param height 矩形高度
	 * @param radiusX X轴方向圆角半径，默认值为：0，为不带圆角矩形
	 * @param radiusY Y轴方向圆角半径，默认值为：0，为不带圆角矩形
	 */
	void fillRect(int left, int top, int width, int height, int radiusX = 0, int radiusY = 0);

	/**
	 * @brief 填充圆弧
	 * @param centerX 中心点X坐标
	 * @param centerY 中心点Y坐标
	 * @param radiusX X轴方向半径
	 * @param radiusY Y轴方向半径，默认值为：0，表示X与Y方向半径相等，为圆形，否则为椭圆形
	 * @param startAngle 起始角度，垂直方向向上为0度方向，顺时针方向为正，逆时针方向为负； 默认值为：0度
	 * @param sweepAngle 偏移角度，顺时针方向为正，逆时针方向为负； 默认值为：360度
	 */
	void fillArc(int centerX, int centerY, int radiusX, int radiusY = 0, int startAngle = 0, int sweepAngle = 360);

	/**
	 * @brief 画直线
	 * @param pPoints 坐标数组
	 * @param count 数组长度
	 */
	void drawLines(const MPPOINT *pPoints, int count);

	/**
	 * @brief 画曲线
	 * @param pPoints 坐标数组
	 * @param count 数组长度
	 */
	void drawCurve(const MPPOINT *pPoints, int count);

	/**
	 * @brief 擦除
	 * @param pRect 区域位置，默认值为：NULL，表示擦除整个控件
	 */
	void erase(const RECT *pRect = NULL);

protected:
	ZKPainter(HWND hParentWnd, ZKBasePrivate *pBP);

	virtual void onBeforeCreateWindow(const Json::Value &json);
	virtual const char* getClassName() const { return ZK_PAINTER; }

	virtual void onDraw(HDC hdc);

private:
	void parsePainterAttributeFromJson(const Json::Value &json);
};

#endif /* _CONTROL_ZKPAINTER_H_ */

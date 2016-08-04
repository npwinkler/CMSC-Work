/* Nolan Winkler, nolanwinkler */
/*CS 152, Summer 2013 */
/* Lab 3 */

#include <stdio.h>
#include <stdlib.h>
#include "checkit.h"
#define PI 3.141592654

struct point{
	float x;
	float y;
};
typedef struct point * Point;

struct square {
	float side;
	Point center;
};
typedef struct square * Square;

struct rectangle {
	float width;
	float length;
	Point center;
};
typedef struct rectangle * Rectangle;

struct circle {
	float radius;
	Point center;
};
typedef struct circle * Circle;

Point makePoint(float x, float y);
void printPoint(Point thepoint);
Square makeSquare(float side, Point center);
void printSquare(Square thesquare);
Rectangle makeRectangle(float width, float length, Point center);
void printRectangle(Rectangle therectangle);
Circle makeCircle(float radius, Point center);
void printCircle(Circle thecircle);
float rectanglePerimeter(Rectangle theRect);
float rectangleArea(Rectangle theRect);
float squarePerimeter(Square theSqr);
float squareArea(Square theSqr);
float circlePerimeter(Circle theCirc);
float circleArea(Circle theCirc);
void fullPrintSquare(Square someSqr);
void fullPrintRectangle(Rectangle someRect);
void fullPrintCircle(Circle someCirc);

int main(void) {
	Point q_one = makePoint(2.0f, 3.5f);
	Point q_two = makePoint(-2.0f, 3.5f);
	Point q_three = makePoint(-2.0f, -3.5f);
	Point q_four = makePoint(2.0f, -3.5f);
	Square s_one = makeSquare(1.0f, q_one);
	Square s_two = makeSquare(2.2f, q_two);
	Square s_three = makeSquare(5.3f, q_three);
	Square s_four = makeSquare(9, q_four);
	Square s_five = makeSquare(10, makePoint(10, 10));
	Rectangle r_one = makeRectangle(1.0f, 1.0f, q_one);
	Rectangle r_two = makeRectangle(0.1f, 190, q_two);
	Rectangle r_three = makeRectangle(2.3f, 5/2, q_three);
	Rectangle r_four = makeRectangle(0, 0, q_four);
	Rectangle r_five = makeRectangle(10, 10, makePoint(10,10));
	Circle c_one = makeCircle(1.0f, q_one);
	Circle c_two = makeCircle(2.4f, q_two);
	Circle c_three = makeCircle(99, q_three);
	Circle c_four = makeCircle (12.3f, q_four);
	Circle c_five = makeCircle(10, makePoint(5,5));
	printPoint(q_one);
	printPoint(q_two);
	printPoint(q_three);
	printPoint(q_four);
	printSquare(s_one);
	printSquare(s_two);
	printSquare(s_three);
	printSquare(s_four);
	printRectangle(r_one);
	printRectangle(r_two);
	printRectangle(r_three);
	printRectangle(r_four);
	printCircle(c_one);
	printCircle(c_two);
	printCircle(c_three);
	printCircle(c_four);
	printf("Square one has perimeter %f and area %f\n",
	squarePerimeter(s_one), squareArea(s_one));
	printf("Rectangle one has perimeter %f and area %f\n",
	rectanglePerimeter(r_one), rectangleArea(r_one));
	printf("Circle one has perimeter %f and area %f\n",
	circlePerimeter(c_one), circleArea(c_one));
	checkit_float(squarePerimeter(s_one), 4.0f);
	checkit_float(squareArea(s_one), 1.0f);
	checkit_float(rectanglePerimeter(r_one), 4.0f);
	checkit_float(rectangleArea(r_one), 1.0f);
	checkit_float(circlePerimeter(c_one), 2*PI);
	checkit_float(circleArea(c_one), PI);
	fullPrintSquare(s_five);
	fullPrintRectangle(r_five);
	fullPrintCircle(c_five);
}

Point makePoint(float x, float y) {
	Point newpoint = (Point)malloc(sizeof(struct point));
	newpoint->x = x;
	newpoint->y = y;
	return newpoint;
}

void printPoint(Point thepoint) {
	printf("(%f, %f)\n", thepoint->x, thepoint->y);
}

Square makeSquare(float side, Point center) {
	Square newsquare = (Square)malloc(sizeof(struct square));
	newsquare->side = side;
	newsquare->center = center;
	return newsquare;
}

void printSquare(Square thesquare) {
	printf("Square with side length %f and center at ", thesquare->side);
	printPoint(thesquare->center);
}

Rectangle makeRectangle(float width, float length, Point center) {
	Rectangle newrectangle = (Rectangle)malloc(sizeof(struct rectangle));
	newrectangle->width = width;
	newrectangle->length = length;
	newrectangle->center = center;
	return newrectangle;
}

void printRectangle(Rectangle therectangle) {
	printf("Rectangle with width %f, length %f and center at ",
	therectangle->width, therectangle->length);
	printPoint(therectangle->center);
}

Circle makeCircle(float radius, Point center) {
	Circle newcircle = (Circle)malloc(sizeof(struct circle));
	newcircle->radius = radius;
	newcircle->center = center;
	return newcircle;
}

void printCircle(Circle thecircle) {
	printf("Circle with radius %f and center at ", thecircle->radius);
	printPoint(thecircle->center);
}

float squarePerimeter(Square theSqr) {
	return 4 * theSqr->side;
}

float squareArea(Square theSqr) {
	return theSqr->side * theSqr->side;
}

float rectanglePerimeter(Rectangle theRect) {
	return 2 * (theRect->width + theRect->length);
}

float rectangleArea(Rectangle theRect) {
	return theRect->width * theRect->length;
}

float circlePerimeter(Circle theCirc) {
	return 2 * PI * theCirc->radius;
}

float circleArea(Circle theCirc) {
	return PI * theCirc->radius * theCirc->radius;
}

void fullPrintSquare(Square thesquare) {
	printSquare(thesquare);
	printf("has perimeter %f and area %f.\n",
	squarePerimeter(thesquare), squareArea(thesquare));
	}

void fullPrintRectangle(Rectangle therect) {
	printRectangle(therect);
	printf("has perimeter %f and area %f.\n",
	rectanglePerimeter(therect), rectangleArea(therect));
}

void fullPrintCircle(Circle thecirc) {
	printCircle(thecirc);
	printf("has perimeter %f and area %f.\n",
	circlePerimeter(thecirc), circleArea(thecirc));
}
 


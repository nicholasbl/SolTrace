#include "bbox_calculator.hpp"

#include <algorithm>

namespace SolTrace::EmbreeRunner
{

    using SolTrace::NativeRunner::TElement;
    using SolTrace::NativeRunner::TStage;

    float get_absolute_minmax(const float values[], int size, bool is_max)
    {
        float minmax_abs = std::abs(values[0]);

        if (is_max)
        {
            for (int i = 1; i < size; i++)
            {
                minmax_abs = std::max(std::abs(values[i]),
                                      minmax_abs);
            }
        }
        else
        {
            for (int i = 1; i < size; i++)
            {
                minmax_abs = std::min(std::abs(values[i]),
                                      minmax_abs);
            }
        }

        return minmax_abs;
    }

    // void process_zernike_bounds(TElement* st_element, float x_minmax[2], float y_minmax[2],
    // 	float(&z_minmax)[2])
    // {
    // 	double z, x, y;

    // 	float& z_min = z_minmax[0];
    // 	float& z_max = z_minmax[1];

    // 	z_min = std::numeric_limits<double>::infinity();
    // 	z_max = -std::numeric_limits<double>::infinity();

    // 	float x_range[3] = { x_minmax[0], x_minmax[1], 0.f };
    // 	float y_range[3] = { y_minmax[0], y_minmax[1], 0.f };

    // 	for (int xi = 0; xi < 3; ++xi)
    // 	{
    // 		for (int yi = 0; yi < 3; ++yi)
    // 		{
    // 			x = x_range[xi];
    // 			y = y_range[yi];
    // 			EvalMono(x, y, st_element->BCoefficients, st_element->FitOrder, 0.0, 0.0, &z);

    // 			if (z < z_min) z_min = z;
    // 			if (z > z_max) z_max = z;
    // 		}
    // 	}
    // }

    // void process_poly_bounds(TElement* st_element, float x_minmax[2], float y_minmax[2],
    // 	float(&z_minmax)[2])
    // {
    // 	double z, x, y;

    // 	float& z_min = z_minmax[0];
    // 	float& z_max = z_minmax[1];

    // 	z_min = std::numeric_limits<double>::infinity();
    // 	z_max = -std::numeric_limits<double>::infinity();

    // 	float x_range[3] = { x_minmax[0], x_minmax[1], 0.f };
    // 	float y_range[3] = { y_minmax[0], y_minmax[1], 0.f };

    // 	for (int xi = 0; xi < 3; ++xi)
    // 	{
    // 		for (int yi = 0; yi < 3; ++yi)
    // 		{
    // 			x = x_range[xi];
    // 			y = y_range[yi];
    // 			EvalPoly(x, y, st_element->PolyCoeffs, st_element->FitOrder, &z);

    // 			if (z < z_min) z_min = z;
    // 			if (z > z_max) z_max = z;
    // 		}
    // 	}
    // }

    // bool find_spline_extrema(std::vector<double>& xa,
    // 	std::vector<double>& ya,
    // 	std::vector<double>& y2a,
    // 	double xMin, double xMax,
    // 	double& yMin, double& yMax)
    // {
    // 	if (xa.size() < 2 || xa.size() != ya.size() || xa.size() != y2a.size())
    // 		return false;

    // 	int n = xa.size();
    // 	yMin = INFINITY;
    // 	yMax = -INFINITY;

    // 	// Check all interval critical points
    // 	for (int i = 0; i < n - 1; i++) {
    // 		double h = xa[i + 1] - xa[i];
    // 		if (h == 0) continue;

    // 		// The spline derivative equal to zero gives us a quadratic equation
    // 		// Coefficients derived from the derivative expression in splint function
    // 		double A = h * (y2a[i + 1] - y2a[i]) / 2.0;
    // 		double B = h * y2a[i] / 2.0 - (ya[i + 1] - ya[i]) / h;
    // 		double C = -h * y2a[i] / 6.0;

    // 		// Solve quadratic equation: A*t² + B*t + C = 0 where t = (x-xa[i])/h
    // 		double discriminant = B * B - 4 * A * C;

    // 		if (std::abs(A) < 1e-10) {
    // 			// Linear case
    // 			if (std::abs(B) > 1e-10) {
    // 				double t = -C / B;
    // 				double x = xa[i] + t * h;
    // 				if (x >= xMin && x <= xMax && x >= xa[i] && x <= xa[i + 1]) {
    // 					double y, dydx;
    // 					if (splint(xa, ya, y2a, n, x, &y, &dydx)) {
    // 						yMin = std::min(yMin, y);
    // 						yMax = std::max(yMax, y);
    // 					}
    // 				}
    // 			}
    // 		}
    // 		else if (discriminant >= 0) {
    // 			// Two possible roots
    // 			double t1 = (-B + sqrt(discriminant)) / (2 * A);
    // 			double t2 = (-B - sqrt(discriminant)) / (2 * A);

    // 			double x1 = xa[i] + t1 * h;
    // 			double x2 = xa[i] + t2 * h;

    // 			// Check if critical points are in this interval and the overall range
    // 			if (x1 >= xa[i] && x1 <= xa[i + 1] && x1 >= xMin && x1 <= xMax) {
    // 				double y, dydx;
    // 				if (splint(xa, ya, y2a, n, x1, &y, &dydx)) {
    // 					yMin = std::min(yMin, y);
    // 					yMax = std::max(yMax, y);
    // 				}
    // 			}

    // 			if (x2 >= xa[i] && x2 <= xa[i + 1] && x2 >= xMin && x2 <= xMax) {
    // 				double y, dydx;
    // 				if (splint(xa, ya, y2a, n, x2, &y, &dydx)) {
    // 					yMin = std::min(yMin, y);
    // 					yMax = std::max(yMax, y);
    // 				}
    // 			}
    // 		}
    // 	}

    // 	// Also check endpoints and knot points within range
    // 	for (int i = 0; i < n; i++) {
    // 		if (xa[i] >= xMin && xa[i] <= xMax) {
    // 			double y, dydx;
    // 			if (splint(xa, ya, y2a, n, xa[i], &y, &dydx)) {
    // 				yMin = std::min(yMin, y);
    // 				yMax = std::max(yMax, y);
    // 			}
    // 		}
    // 	}

    // 	// Check range endpoints if they're not knot points
    // 	double y, dydx;
    // 	if (splint(xa, ya, y2a, n, xMin, &y, &dydx)) {
    // 		yMin = std::min(yMin, y);
    // 		yMax = std::max(yMax, y);
    // 	}

    // 	if (splint(xa, ya, y2a, n, xMax, &y, &dydx)) {
    // 		yMin = std::min(yMin, y);
    // 		yMax = std::max(yMax, y);
    // 	}

    // 	// Check x = 0;
    // 	if (splint(xa, ya, y2a, n, 0, &y, &dydx)) {
    // 		yMin = std::min(yMin, y);
    // 		yMax = std::max(yMax, y);
    // 	}

    // 	return yMin != INFINITY && yMax != -INFINITY;
    // }

    // void process_cubic_spline_bounds(TElement* st_element, float x_minmax[2], float y_minmax[2],
    // 	float(&z_minmax)[2])
    // {
    // 	double z, x, y;

    // 	float& z_min = z_minmax[0];
    // 	float& z_max = z_minmax[1];

    // 	z_min = std::numeric_limits<double>::infinity();
    // 	z_max = -std::numeric_limits<double>::infinity();

    // 	float x_min_abs = get_absolute_minmax(x_minmax, 2, false);
    // 	float x_max_abs = get_absolute_minmax(x_minmax, 2, true);
    // 	float y_min_abs = get_absolute_minmax(y_minmax, 2, false);
    // 	float y_max_abs = get_absolute_minmax(y_minmax, 2, true);

    // 	double Rho_min = sqrt(x_min_abs * x_min_abs + y_min_abs * y_min_abs);
    // 	double Rho_max = sqrt(x_max_abs * x_max_abs + y_max_abs * y_max_abs);
    // 	double z_min_test;
    // 	double z_max_test;

    // 	find_spline_extrema(st_element->CubicSplineXData,
    // 		st_element->CubicSplineYData,
    // 		st_element->CubicSplineY2Data,
    // 		Rho_min, Rho_max, z_min_test, z_max_test);

    // 	/*for (int xi = 0; xi < 3; ++xi)
    // 	{
    // 		for (int yi = 0; yi < 3; ++yi)
    // 		{
    // 			x = x_range[xi];
    // 			y = y_range[yi];
    // 			double Rho = sqrt(x * x + y * y);
    // 			double dummy;
    // 			splint(st_element->CubicSplineXData,
    // 				st_element->CubicSplineYData,
    // 				st_element->CubicSplineY2Data,
    // 				st_element->CubicSplineXData.size(),
    // 				Rho, &z, &dummy);

    // 			if (z < z_min) z_min = z;
    // 			if (z > z_max) z_max = z;
    // 		}
    // 	}*/

    // 	z_min = z_min_test;
    // 	z_max = z_max_test;
    // }

    // void process_FE_bounds(TElement* st_element, float x_minmax[2], float y_minmax[2],
    // 	float(&z_minmax)[2])
    // {
    // 	float& z_min = z_minmax[0];
    // 	float& z_max = z_minmax[1];

    // 	z_min = std::numeric_limits<double>::infinity();
    // 	z_max = -std::numeric_limits<double>::infinity();

    // 	const MatDoub& xyz_nodes = st_element->FEData.nodes;

    // 	for (const std::vector<double>& fe_node : xyz_nodes)
    // 	{
    // 		if (fe_node[2] > z_max)
    // 			z_max = fe_node[2];
    // 		if (fe_node[2] < z_min)
    // 			z_min = fe_node[2];
    // 	}
    // }

    void transform_to_global(const float coord_element[3],
                             TStage *st_stage, TElement *st_element,
                             float (&coord_global)[3])
    {
        // float PosDumStage[3];
        // float coord_stage[3];
        // MatrixVectorMult_generic(st_element->RLocToRef, coord_element, PosDumStage);
        // for (int i = 0; i < 3; i++)
        // 	coord_stage[i] = PosDumStage[i] + st_element->Origin[i];

        // float PosDumGlob[3];
        // MatrixVectorMult_generic(st_stage->RLocToRef, coord_stage, PosDumGlob);
        // for (int i = 0; i < 3; i++)
        // 	coord_global[i] = PosDumGlob[i] + st_stage->Origin[i];
        return;
    }

    void transform_bounds(const float min_coord_element[3],
                          const float max_coord_element[3],
                          TStage *st_stage,
                          TElement *st_element,
                          float (&min_coord_global)[3],
                          float (&max_coord_global)[3])
    {
        // Transform min and max bounding box from element coordinates to global
        float corners_element[8][3] =
            {
                {min_coord_element[0], min_coord_element[1], min_coord_element[2]},
                {min_coord_element[0], min_coord_element[1], max_coord_element[2]},
                {min_coord_element[0], max_coord_element[1], min_coord_element[2]},
                {min_coord_element[0], max_coord_element[1], max_coord_element[2]},
                {max_coord_element[0], min_coord_element[1], min_coord_element[2]},
                {max_coord_element[0], min_coord_element[1], max_coord_element[2]},
                {max_coord_element[0], max_coord_element[1], min_coord_element[2]},
                {max_coord_element[0], max_coord_element[1], max_coord_element[2]}};

        // Convert corners to global coordinates
        float corners_global[8][3];
        for (int i = 0; i < 8; i++)
            transform_to_global(corners_element[i], st_stage, st_element, corners_global[i]);

        // Find min and max xyz
        min_coord_global[0] = corners_global[0][0];
        min_coord_global[1] = corners_global[0][1];
        min_coord_global[2] = corners_global[0][2];
        max_coord_global[0] = corners_global[0][0];
        max_coord_global[1] = corners_global[0][1];
        max_coord_global[2] = corners_global[0][2];
        for (int i = 1; i < 8; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                float val = corners_global[i][j];
                if (val < min_coord_global[j])
                    min_coord_global[j] = val;
                if (val > max_coord_global[j])
                    max_coord_global[j] = val;
            }
        }
    }

    int get_aperture_bounds(TElement *st_element, float &x_min, float &x_max,
                            float &y_min, float &y_max)
    {
        // switch (std::tolower(st_element->ShapeIndex))
        // {
        // case ('c'):
        // {
        //     // Circular
        //     float r = st_element->ParameterA * 0.5f;
        //     x_min = -r;
        //     x_max = r;
        //     y_min = -r;
        //     y_max = r;

        //     break;
        // }
        // case ('h'):
        // {
        //     // Hexagonal
        //     float r = st_element->ParameterA * 0.5f;
        //     float apothem = (2.f * r * std::sqrt(3.f)) / 4.f;
        //     x_min = -r;
        //     x_max = r;
        //     y_min = -apothem;
        //     y_max = apothem;

        //     break;
        // }
        // case ('t'):
        // {
        //     // Triangular
        //     float r = st_element->ParameterA * 0.5f;
        //     float side = r * std::sqrt(3.f);
        //     float h = 1.5f * r;
        //     x_min = -0.5f * side;
        //     x_max = 0.5f * side;
        //     y_min = r - h;
        //     y_max = r;

        //     break;
        // }
        // case ('r'):
        // {
        //     // Rectangular
        //     x_min = st_element->ParameterA * -0.5f;
        //     x_max = st_element->ParameterA * 0.5f;
        //     y_min = st_element->ParameterB * -0.5f;
        //     y_max = st_element->ParameterB * 0.5f;

        //     break;
        // }
        // case ('l'):
        // {
        //     // Single axis curvature section
        //     x_min = st_element->ParameterA;
        //     x_max = st_element->ParameterB;
        //     y_min = st_element->ParameterC * -0.5f;
        //     y_max = st_element->ParameterC * 0.5f;

        //     break;
        // }
        // case ('a'):
        // {
        //     // Annular
        //     float r_inner = st_element->ParameterA;
        //     float r_outer = st_element->ParameterB;
        //     float theta_deg = st_element->ParameterC;

        //     // IGNORING theta for now
        //     x_min = -1.f * r_outer;
        //     x_max = r_outer;
        //     y_min = -1.f * r_outer;
        //     y_max = r_outer;

        //     break;
        // }
        // case ('i'):
        // {
        //     // Irregular triangle
        //     float x1 = st_element->ParameterA;
        //     float y1 = st_element->ParameterB;
        //     float x2 = st_element->ParameterC;
        //     float y2 = st_element->ParameterD;
        //     float x3 = st_element->ParameterE;
        //     float y3 = st_element->ParameterF;

        //     // Put all x and y values in arrays
        //     float x_values[3] = {x1, x2, x3};
        //     float y_values[3] = {y1, y2, y3};

        //     // Find min and max using std::min_element and std::max_element
        //     x_min = *std::min_element(x_values, x_values + 3);
        //     x_max = *std::max_element(x_values, x_values + 3);
        //     y_min = *std::min_element(y_values, y_values + 3);
        //     y_max = *std::max_element(y_values, y_values + 3);

        //     break;
        // }
        // case ('q'):
        // {
        //     // Irregular quadrilateral
        //     float x1 = st_element->ParameterA;
        //     float y1 = st_element->ParameterB;
        //     float x2 = st_element->ParameterC;
        //     float y2 = st_element->ParameterD;
        //     float x3 = st_element->ParameterE;
        //     float y3 = st_element->ParameterF;
        //     float x4 = st_element->ParameterG;
        //     float y4 = st_element->ParameterH;

        //     // Put all x and y values in arrays
        //     float x_values[4] = {x1, x2, x3, x4};
        //     float y_values[4] = {y1, y2, y3, y4};

        //     // Find min and max using std::min_element and std::max_element
        //     x_min = *std::min_element(x_values, x_values + 4);
        //     x_max = *std::max_element(x_values, x_values + 4);
        //     y_min = *std::min_element(y_values, y_values + 4);
        //     y_max = *std::max_element(y_values, y_values + 4);

        //     break;
        // }
        // default:
        // {
        //     x_min = std::numeric_limits<float>::quiet_NaN();
        //     x_max = std::numeric_limits<float>::quiet_NaN();
        //     y_min = std::numeric_limits<float>::quiet_NaN();
        //     y_max = std::numeric_limits<float>::quiet_NaN();
        //     return -1;
        //     break;
        // }
        // }

        return 0;
    }

    int get_surface_bounds(TElement *st_element,
                           float x_minmax[2],
                           float y_minmax[2],
                           float &z_min,
                           float &z_max)
    {

        // switch (std::tolower(st_element->SurfaceIndex))
        // {
        // case 's':
        // {
        //     // Spherical
        //     float c = st_element->VertexCurvX;
        //     float r = 1.f / c;

        //     float x_abs_max = get_absolute_minmax(x_minmax, 2, true);
        //     float y_abs_max = get_absolute_minmax(y_minmax, 2, true);

        //     // Check if max x,y combo is outside sphere
        //     float x, y;
        //     float in_root = 1.f - 1.f - (c * c) * (x_abs_max * x_abs_max + y_abs_max * y_abs_max);
        //     if (in_root <= 0)
        //     {
        //         z_max = r;
        //         z_min = 0.f;
        //     }
        //     else
        //     {
        //         float z_numerator = (c * (x_abs_max * x_abs_max + y_abs_max * y_abs_max));
        //         float z_denom = 1.f + std::sqrt(1.f - (c * c) * (x_abs_max * x_abs_max + y_abs_max * y_abs_max));

        //         z_max = z_numerator / z_denom;
        //         z_min = 0.f;
        //     }

        //     break;
        // }
        // case 'p':
        // {
        //     // Parabolic
        //     float cx = st_element->VertexCurvX;
        //     float cy = st_element->VertexCurvY;

        //     float x_abs_max = get_absolute_minmax(x_minmax, 2, true);
        //     float y_abs_max = get_absolute_minmax(y_minmax, 2, true);

        //     z_max = 0.5f * (cx * x_abs_max * x_abs_max + cy * y_abs_max * y_abs_max);
        //     z_min = 0.f;

        //     break;
        // }
        // case 'o':
        // {
        //     // Hyperboloids and hemiellipsoids
        //     float c = st_element->VertexCurvX;
        //     float kappa = st_element->Kappa;

        //     float x_abs_max = get_absolute_minmax(x_minmax, 2, true);
        //     float y_abs_max = get_absolute_minmax(y_minmax, 2, true);

        //     float z_numer = c * (x_abs_max * x_abs_max + y_abs_max * y_abs_max);
        //     float z_denom = 1.f + std::sqrt(1.f - (kappa * c * c) * (x_abs_max * x_abs_max + y_abs_max * y_abs_max));

        //     z_max = z_numer / z_denom;
        //     z_min = 0.f;

        //     break;
        // }
        // case 'f':
        // {
        //     // Flat
        //     z_max = 1.e-4f;
        //     z_min = -1.e-4f;

        //     break;
        // }
        // case 'c':
        // {
        //     // Conical
        //     float theta_deg = st_element->ConeHalfAngle;
        //     float theta_rad = theta_deg * (M_PI / 180.0f);

        //     float x_abs_max = get_absolute_minmax(x_minmax, 2, true);
        //     float y_abs_max = get_absolute_minmax(y_minmax, 2, true);

        //     z_max = std::sqrt(x_abs_max * x_abs_max + y_abs_max * y_abs_max) / std::tan(theta_rad);
        //     z_min = 0.f;

        //     break;
        // }
        // case 't':
        // {
        //     // Cylindrical (only works with l aperture)
        //     float inverse_R = st_element->CurvOfRev;
        //     float R = 1.f / inverse_R;

        //     // OVERWRITES X BOUNDS
        //     x_minmax[0] = -R;
        //     x_minmax[1] = R;

        //     z_max = 2.f * R;
        //     z_min = 0.f;

        //     break;
        // }
        // case 'm':
        // case 'v':
        // {
        //     // Zernike series file (m) or VSHOT (v)
        //     float z_minmax[2] = {0, 0};
        //     process_zernike_bounds(st_element, x_minmax, y_minmax, z_minmax);

        //     z_min = z_minmax[0];
        //     z_max = z_minmax[1];

        //     break;
        // }
        // case 'r':
        // {
        //     // Rotationally symmetric polynomial file
        //     float z_minmax[2] = {0, 0};
        //     process_poly_bounds(st_element, x_minmax, y_minmax, z_minmax);

        //     z_min = z_minmax[0];
        //     z_max = z_minmax[1];

        //     break;
        // }
        // case 'i':
        // {
        //     // Rotationally symmetric cubic spline file
        //     float z_minmax[2] = {0, 0};
        //     process_cubic_spline_bounds(st_element, x_minmax, y_minmax, z_minmax);

        //     z_min = z_minmax[0] - 10000;
        //     z_max = z_minmax[1];

        //     break;
        // }
        // case 'e':
        // {
        //     // Finite element data
        //     float z_minmax[2] = {0, 0};
        //     process_FE_bounds(st_element, x_minmax, y_minmax, z_minmax);

        //     z_min = z_minmax[0];
        //     z_max = z_minmax[1];

        //     break;
        // }
        // default:
        // {
        //     // not supported for embree
        //     z_min = std::numeric_limits<float>::quiet_NaN();
        //     z_max = std::numeric_limits<float>::quiet_NaN();
        //     return -1;
        // }
        // }

        return 0;
    }

    BBOXERRORS get_bounds(TElement *st_element,
                          float (&min_coord_global)[3],
                          float (&max_coord_global)[3])
    {
        // Get stage
        // TStage *st_stage = st_element->parent_stage;
        TStage *st_stage = nullptr;

        // Define element coord bounds
        float min_coord_element[3] = {0.f, 0.f, 0.f};
        float max_coord_element[3] = {0.f, 0.f, 0.f};

        float x_minmax[2] = {0.f, 0.f};
        float y_minmax[2] = {0.f, 0.f};
        float z_minmax[2] = {0.f, 0.f};

        // Process aperture bounds (sets x and y)
        int error_code = get_aperture_bounds(st_element,
                                             x_minmax[0], x_minmax[1],
                                             y_minmax[0], y_minmax[1]);
        if (error_code != 0)
        {
            // bounds_error(args, "Invalid aperture");
            return BBOXERRORS::BOUNDS_APERTURE_ERROR;
        }

        // Process surface bounds (sets y, and possibly overwrites x and y)
        error_code = get_surface_bounds(st_element,
                                        x_minmax,
                                        y_minmax,
                                        z_minmax[0],
                                        z_minmax[1]);
        if (error_code != 0)
        {
            // bounds_error(args, "Invalid surface");
            return BBOXERRORS::BOUNDS_SURFACE_ERROR;
        }

        // Expand bounding boxes slightly to account for float precision
        float expand = 0.001;
        x_minmax[0] -= expand;
        x_minmax[1] += expand;
        y_minmax[0] -= expand;
        y_minmax[1] += expand;
        z_minmax[0] -= expand;
        z_minmax[1] += expand;

        // Assign points to min/max coordinate element arrays
        min_coord_element[0] = x_minmax[0];
        min_coord_element[1] = y_minmax[0];
        min_coord_element[2] = z_minmax[0];
        max_coord_element[0] = x_minmax[1];
        max_coord_element[1] = y_minmax[1];
        max_coord_element[2] = z_minmax[1];

        // Convert local element bounds, to global xyz
        transform_bounds(min_coord_element, max_coord_element,
                         st_stage, st_element,
                         min_coord_global, max_coord_global);

        return BBOXERRORS::NONE;
    }

} // namespace SolTrace::EmbreeRunner

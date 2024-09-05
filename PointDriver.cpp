#include <cfloat>
#include <fstream>
#include <iostream>
#include <vector>
#include <utility>
#include "Point.h"
#include <algorithm>
#include <functional>

using namespace std;

//brute force approach to cover all the distances
pair<Point, Point> bruteForce(vector<Point>& vec, unsigned int left, unsigned int right) {
    double minDist = DBL_MAX;
    pair<Point, Point> result;
    for (unsigned int i = left; i < right; i++)
        for (unsigned int j = i + 1; j < right; j++) {
            if (vec[i].distance(vec[j]) < minDist) {
                minDist = vec[i].distance(vec[j]);
                result = make_pair(vec[i], vec[j]);
            }
        }
    return result;
}

// Function to calculate the distance between two points in a pair
double distBetweenPoints(pair<Point, Point>& pointPair) {
    return pointPair.first.distance(pointPair.second);
}

// Merge sort function for sorting a vector of Comparable objects based on a comparator
template <typename Comparable>
void mergeSort(vector<Comparable>& a, vector<Comparable>& tmp, int left, int right, function<bool(const Point& p1, const Point& p2)> comparator) {
    if (left < right) {
        int mid = (left + right) / 2;

        mergeSort(a, tmp, left, mid, comparator);
        mergeSort(a, tmp, mid + 1, right, comparator);
        merge(a, tmp, left, mid + 1, right, comparator);
    }
}

// Merge function for merging two sorted subarrays
template <typename Comparable>
void merge(vector<Comparable>& a, vector<Comparable>& temp, int left, int right, int rightEnd, function<bool(const Point& p1, const Point& p2)> comparator) {
    int leftEnd = right - 1;
    int tmpPos = left;
    int numElements = rightEnd - left + 1;

    // Merge the two subarrays while maintaining order based on the comparator function
    while (left <= leftEnd && right <= rightEnd) {
        if (comparator(a[left], a[right])) {
            temp[tmpPos++] = std::move(a[left++]);
        }
        else {
            temp[tmpPos++] = std::move(a[right++]);
        }
    }
    // Copy the remaining elements from the left subarray, if any
    while (left <= leftEnd) { temp[tmpPos++] = std::move(a[left++]); }
    // Copy the remaining elements from the right subarray, if any
    while (right <= rightEnd) { temp[tmpPos++] = std::move(a[right++]); }

    // Copy the sorted elements back to the original array
    for (int i = 0; i < numElements; ++i, --rightEnd) {
        a[rightEnd] = std::move(temp[rightEnd]);
    }

}

//nearest points NlogN function that return pair fo two Points
pair<Point, Point> smallestDistance(vector<Point>& points, int left, int right) {
    //if vector size is less than 4 use bruteforce
    if (right - left < 5) {
        return bruteForce(points, left, right);
    }
    //find the middle and determine smallest elements on both sides of vector
    int mid = (left + right) / 2;
    pair<Point, Point> dlPair = smallestDistance(points, left, mid);
    pair<Point, Point> drPair = smallestDistance(points, mid + 1, right);
    double dl = distBetweenPoints(dlPair);
    double dr = distBetweenPoints(drPair);
    double d = min(dl, dr);

    // Combine the results from the left and right halves
    pair<Point, Point> result;
    if (dl < dr) {
        result = dlPair;
    }
    else {
        result = drPair;
    }

    // Assigning iterators
    unsigned int i, j;

    // Put points on the border of the division to the strip vector
    vector<Point> strip;
    for (i = left; i < right; i++) {
        if (abs(points[i].getX() - points[mid].getX()) < d) {
            strip.push_back(points[i]);
        }
    }
    // Sort strip according to Y coordinates
    vector<Point> u(strip.size());
    mergeSort(strip, u, 0, strip.size() - 1, Point::CompareYCoordinate());

    // Find the smallest distance in the strip
    size_t size = strip.size();
    for (i = 0; i < size; i++) {
        for (j = i + 1; j < size && strip[j].getY() - strip[i].getY() < d; j++) {
            double distance = strip[j].distance(strip[i]);
            if (distance < d) {
                d = distance;
                result = make_pair(strip[i], strip[j]);
            }
        }
    }
    return result;
}

pair<Point, Point> closest(vector<Point> points) {
    vector<Point> sortedByX = points;
    mergeSort(points, sortedByX, 0, points.size() - 1, Point::CompareXCoordinate());
    vector<Point> sortedByY = points;
    mergeSort(points, sortedByY, 0, points.size() - 1, Point::CompareYCoordinate());
    return smallestDistance(sortedByX, 0, points.size());
}


int main() {
    ifstream file;
    string filename;
    cout << "Enter Filename: ";
    cin >> filename;
    file.open(filename);
    if (filename.length() >= 4 && filename.substr(filename.length() - 4) == ".txt") {
        ifstream file(filename);
        if (!file) {
            cout << "Error opening the file" << endl;
            return 1;
        }
    }
    else {
        cout << "Invalid file extension, you should provide .txt files" << endl;
        return 1;
    }
    vector<Point> points;
    Point pt;
    while (file >> pt) {
        points.emplace_back(pt);
    }
    file.close();

    // Check if the number of points read is greater than 1
    if (points.size() < 2) {
        cout << "Error: at least 2 points needed" << endl;
        return 1;
    }

    pair<Point, Point> pts = closest(points);
    cout << "Closest points are: (" << pts.first.getX() << ", " << pts.first.getY()
        << ") and (" << pts.second.getX() << ", " << pts.second.getY() << ") with distance = "
        << distBetweenPoints(pts) << endl;

    return 0;
}
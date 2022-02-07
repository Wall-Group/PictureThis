#ifndef PICTURETHIS_INC_DRAWING_H_
#define PICTURETHIS_INC_DRAWING_H_

// pt stands for picture this
namespace pt {

enum class Color {
    kErase = 0,
    kWhite,
    kRed,
    kGreen,
    kBlue,
    kOrange,
    kYellow,
    kPurple
};

enum class Mode {
    kDefault = 0,
    kDrawing
};

enum class Cursor {
    kDefault = 0,
    kSquare,
    kCross
};

class Drawing {
public:
    Drawing();
    ~Drawing();

private:
    Color color_;
    Mode mode_;
    Cursor cursor_;

};
}  // namespace pt
#endif  // PICTURETHIS_INC_DRAWING_H_
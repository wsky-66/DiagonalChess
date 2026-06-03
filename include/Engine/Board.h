#pragma once

class Board {
public:
    virtual ~Board() = default;
    virtual int GetRows() const = 0;
    virtual int GetCols() const = 0;
    virtual void Reset() = 0;
};

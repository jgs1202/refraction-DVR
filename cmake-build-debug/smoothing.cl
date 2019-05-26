bool checkRange(int x, int side) {
    if ((x >= 0) && (x <= side - 1)) {
        return true;
    } else {
        return false;
    }
}

bool checkInside(int x, int y, int z, int side) {
    if (checkRange(x, side) && checkRange(y, side) && checkRange(z, side)) {
        return true;
    } else {
        return false;
    }
}

int getz (int p, int side) {
    return (int)(p / (side * side));
}

int gety (int p, int side) {
    return (int)((p - getz(p, side) * side * side) / side);
}

int getx (int p, int side) {
    return p - getz(p, side) * side * side - gety(p, side) * side;
}

int coordinate(int x, int y, int z, int side) {
    return x + y * side + z * side * side;
}

__kernel void smooth (__global float *grad, int side){
    int position = get_global_id(0);
    int x = getx(position, side), y = gety(position, side), z = getz(position, side), xx, yy, zz;
    float newGrad[] = {0, 0, 0};
    float gauss[27] = {0, 1, 0, 1, 2, 1, 0, 1, 0, 1, 2, 1, 2, 4, 2, 1, 2, 1, 0, 1, 0, 1, 2, 1, 0, 1, 0};
    int kernelSz = 3, halfKernel = 1, count = 0;

    for(int i=0; i < kernelSz; ++i){
        for (int j=0; j < kernelSz; ++j){
            for (int k=0; k < kernelSz; ++k){
                xx = x - halfKernel + i;
                yy = y - halfKernel + j;
                zz = z - halfKernel + k;
                if (checkInside(xx, yy, zz, side)) {
                    newGrad[0] = newGrad[0] + grad[coordinate(xx, yy, zz, side) * 3] * gauss[i + j * kernelSz + k * kernelSz * kernelSz];
                    newGrad[1] = newGrad[1] + grad[coordinate(xx, yy, zz, side) * 3 + 1] * gauss[i + j * kernelSz + k * kernelSz * kernelSz];
                    newGrad[2] = newGrad[2] + grad[coordinate(xx, yy, zz, side) * 3 + 2] * gauss[i + j * kernelSz + k * kernelSz * kernelSz];
                    count = count + gauss[i + j * kernelSz + k * kernelSz * kernelSz];
                }
            }
        }
    }
    newGrad[0] = newGrad[0] / count;
    newGrad[1] = newGrad[1] / count;
    newGrad[2] = newGrad[2] / count;

    grad[position * 3] = newGrad[0]; 
    grad[position * 3 + 1] = newGrad[1]; 
    grad[position * 3 + 2] = newGrad[2]; 
}
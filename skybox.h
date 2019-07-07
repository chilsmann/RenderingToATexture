#ifndef SKYBOX_H
#define SKYBOX_H


class SkyBox
{
private:
    unsigned int vaoHandle;

public:
    SkyBox();

    void render() const;
};


#endif // SKYBOX_H

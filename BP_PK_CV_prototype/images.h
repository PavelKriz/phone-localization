//----------------------------------------------------------------------------------------
/**
 * \file       images.h
 * \author     Pavel Kriz
 * \date       9/4/2021
 * \brief      File containing vectors with strings containing filepaths to images from the bc. Jan Sefcik's image database
*/
//----------------------------------------------------------------------------------------

#pragma once

#include <vector> 
#include <string>

/// Vector with reference images from bc. Jan Sefcik's database
const vector<string> sefcikImagesObjects{
    "input_images\\sefcik\\b1_obj\\01_b.jpg",   //0
    "input_images\\sefcik\\b1_obj\\02_b.jpg",   //1
    "input_images\\sefcik\\b1_obj\\03_b.jpg",   //2
    "input_images\\sefcik\\b1_obj\\04_b.jpg",   //3
    "input_images\\sefcik\\b2_obj\\01_b.jpg",   //4
    "input_images\\sefcik\\b2_obj\\02_b.jpg",   //5
    "input_images\\sefcik\\b2_obj\\03_b.jpg",   //6
    "input_images\\sefcik\\b2_obj\\04_b.jpg",   //7
    "input_images\\sefcik\\b2_obj\\05_b.jpg",   //8
    "input_images\\sefcik\\b3_obj\\01_b.jpg",   //9
    "input_images\\sefcik\\b3_obj\\02_b.jpg",   //10
    "input_images\\sefcik\\b3_obj\\03_b.jpg",   //11
    "input_images\\sefcik\\b3_obj\\04_b.jpg",   //12
    "input_images\\sefcik\\b4_obj\\01_b.jpg",   //13
    "input_images\\sefcik\\b4_obj\\02_b.jpg",   //14
    "input_images\\sefcik\\b4_obj\\03_b.jpg",   //15
    "input_images\\sefcik\\b4_obj\\04_b.jpg",   //16
    "input_images\\sefcik\\b4_obj\\05_b.jpg",   //17
    "input_images\\sefcik\\b5_obj\\01_b.jpg",   //18
    "input_images\\sefcik\\b5_obj\\02_b.jpg",   //19
    "input_images\\sefcik\\b5_obj\\03_b.jpg",   //20
    "input_images\\sefcik\\b5_obj\\04_b.jpg",   //21
    "input_images\\sefcik\\b5_obj\\05_b.jpg",   //22
    "input_images\\sefcik\\b7_obj\\01_b.jpg",   //23
    "input_images\\sefcik\\b7_obj\\02_b.jpg",   //24
    "input_images\\sefcik\\b7_obj\\03_b.jpg",   //25
    "input_images\\sefcik\\b8_obj\\01_b.jpg",   //26
    "input_images\\sefcik\\b8_obj\\02_b.jpg",   //27
    "input_images\\sefcik\\b8_obj\\03_b.jpg",   //28
    "input_images\\sefcik\\b8_obj\\04_b.jpg",   //29
    "input_images\\sefcik\\b8_obj\\05_b.jpg",   //30
    "input_images\\sefcik\\b8_obj\\06_b.jpg",   //31
    "input_images\\sefcik\\b9_obj\\01_b.jpg",   //32
    "input_images\\sefcik\\b9_obj\\02_b.jpg",   //33
    "input_images\\sefcik\\b9_obj\\03_b.jpg",   //34
    "input_images\\sefcik\\b10_obj\\01_b.jpg",  //35
    "input_images\\sefcik\\b10_obj\\02_b.jpg",  //36
    "input_images\\sefcik\\b10_obj\\03_b.jpg",  //37
    "input_images\\sefcik\\b10_obj\\04_b.jpg",  //38
    "input_images\\sefcik\\b10_obj\\05_b.jpg",  //39
    "input_images\\sefcik\\b11_obj\\01_b.jpg",  //40
    "input_images\\sefcik\\b11_obj\\02_b.jpg",  //41
    "input_images\\sefcik\\b11_obj\\03_b.jpg",  //42
    "input_images\\sefcik\\b11_obj\\04_b.jpg",  //43
    "input_images\\sefcik\\b12_obj\\01_b.jpg",  //44
    "input_images\\sefcik\\b12_obj\\02_b.jpg",  //45
    "input_images\\sefcik\\b12_obj\\03_b.jpg",  //46
    "input_images\\sefcik\\b12_obj\\04_b.jpg"   //47
};

/// Vector with scene images from bc. Jan Sefcik's database
const vector<string> sefcikImagesScenes{
    "input_images\\sefcik\\b1_scn\\01_b.jpg"       , //0
    "input_images\\sefcik\\b1_scn\\01_b_small.jpg" , //1
    "input_images\\sefcik\\b1_scn\\02_b.jpg"       , //2
    "input_images\\sefcik\\b1_scn\\02_b_small.jpg" , //3
    "input_images\\sefcik\\b1_scn\\03.jpg"         , //4
    "input_images\\sefcik\\b1_scn\\04.jpg"         , //5
    "input_images\\sefcik\\b1_scn\\05_b_small.jpg" , //6
    "input_images\\sefcik\\b2_scn\\01.jpg"         , //7
    "input_images\\sefcik\\b2_scn\\02.jpg"         , //8
    "input_images\\sefcik\\b2_scn\\03.jpg"         , //9
    "input_images\\sefcik\\b2_scn\\04.jpg"         , //10
    "input_images\\sefcik\\b2_scn\\05.jpg"         , //11
    "input_images\\sefcik\\b3_scn\\01.jpg"         , //12
    "input_images\\sefcik\\b3_scn\\02.jpg"         , //13
    "input_images\\sefcik\\b3_scn\\03.jpg"         , //14
    "input_images\\sefcik\\b3_scn\\04.jpg"         , //15
    "input_images\\sefcik\\b4_scn\\01.jpg"         , //16
    "input_images\\sefcik\\b4_scn\\02_b_small.jpg" , //17
    "input_images\\sefcik\\b4_scn\\03.jpg"         , //18
    "input_images\\sefcik\\b4_scn\\04.jpg"         , //19
    "input_images\\sefcik\\b4_scn\\05.jpg"         , //20
    "input_images\\sefcik\\b4_scn\\05_b_small.jpg" , //21
    "input_images\\sefcik\\b5_scn\\01.jpg"         , //22
    "input_images\\sefcik\\b5_scn\\01_b_small.jpg" , //23
    "input_images\\sefcik\\b5_scn\\02.jpg"         , //24
    "input_images\\sefcik\\b5_scn\\03.jpg"         , //25
    "input_images\\sefcik\\b5_scn\\04.jpg"         , //26
    "input_images\\sefcik\\b5_scn\\05.jpg"         , //27
    "input_images\\sefcik\\b7_scn\\01_b_small.jpg" , //28
    "input_images\\sefcik\\b7_scn\\02_b_small.jpg" , //29
    "input_images\\sefcik\\b7_scn\\03_b_small.jpg" , //30
    "input_images\\sefcik\\b8_scn\\01_b_small.jpg" , //31
    "input_images\\sefcik\\b8_scn\\02_b_small.jpg" , //32
    "input_images\\sefcik\\b8_scn\\03_b_small.jpg" , //33
    "input_images\\sefcik\\b8_scn\\04_b_small.jpg" , //34
    "input_images\\sefcik\\b8_scn\\05_b_small.jpg" , //35
    "input_images\\sefcik\\b8_scn\\06_b_small.jpg" , //36
    "input_images\\sefcik\\b9_scn\\01_b_small.jpg" , //37
    "input_images\\sefcik\\b9_scn\\02_b_small.jpg" , //38
    "input_images\\sefcik\\b9_scn\\03_b_small.jpg" , //39
    "input_images\\sefcik\\b10_scn\\01_b_small.jpg", //40
    "input_images\\sefcik\\b10_scn\\02_b_small.jpg", //41
    "input_images\\sefcik\\b10_scn\\03_b_small.jpg", //42
    "input_images\\sefcik\\b10_scn\\04_b_small.jpg", //43
    "input_images\\sefcik\\b10_scn\\05_b_small.jpg", //44
    "input_images\\sefcik\\b11_scn\\01_b_small.jpg", //45
    "input_images\\sefcik\\b11_scn\\02_b_small.jpg", //46
    "input_images\\sefcik\\b11_scn\\03_b_small.jpg", //47
    "input_images\\sefcik\\b11_scn\\04_b_small.jpg", //48
    "input_images\\sefcik\\b12_scn\\01_b_small.jpg", //49
    "input_images\\sefcik\\b12_scn\\02_b_small.jpg", //50
    "input_images\\sefcik\\b12_scn\\03_b_small.jpg", //51
    "input_images\\sefcik\\b12_scn\\04_b_small.jpg" //52
};

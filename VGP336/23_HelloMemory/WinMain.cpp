#include <IExeEngine/Inc/IExeEngine.h>

using namespace IExeEngine;
using namespace IExeEngine::Core;

static int sUniqueId = 0;
class Student
{
public:
    Student()
    {
        mId = ++sUniqueId;
        mMark = rand() % 100;
        mName = "Ibrahim_" + std::to_string(mId);
    }
    Student(const std::string& name)
        : mName(name)
    {
        mId = ++sUniqueId;
        mMark = rand() % 100;
    }
    Student(const std::string& name, int mark)
        : mName(name)
    {
        mId = ++sUniqueId;
        mMark = rand() % 100;
    }

    void Log()
    {
        LOG("Name: %s - ID: %d - Mark: %d", mName.c_str(), mId, mMark);
    }

private:
    int mId = 0;
    int mMark = 0;
    std::string mName;
};

int WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int)
{
    TypedAllocator studentPool = TypedAllocator<Student>("StudentPool", 100);

    std::vector<Student*> students;

    // Test New
    for (uint32_t i = 0; i < 50; ++i)
    {
        std::string newStudentName = "NewStudent" + std::to_string(i);
        Student* newStudent = studentPool.New(newStudentName, rand() % 9);
        newStudent->Log();
        students.push_back(newStudent);
    }

    // Test Delete
    for (uint32_t i = 0; i < 30; ++i)
    {
        Student* student = students.back();
        studentPool.Delete(student);
        students.pop_back();
    }

    // Test Add More
    for (uint32_t i = 0; i < 20; ++i)
    {
        Student* newStudent = studentPool.New();
        newStudent->Log();
        students.push_back(newStudent);
    }

    // Clean up remaining students
    for (Student* student : students)
    {
        student->Log();
        studentPool.Delete(student);
    }
    students.clear();

    return 0;
}
#ifndef MODEL_H
#define MODEL_H

#include <QString>
#include <QStringList>
#include <QVector>
#include <QObject>

struct MultipleChoiceOption {
    QString text;
    bool isCorrect;
    
    MultipleChoiceOption(const QString& t = "", bool correct = false)
        : text(t), isCorrect(correct) {}
};

struct Problem {
    QString name;
    QString description;
    QString difficulty;
    QString problemStatement;
    QString theoryContent;
    QVector<MultipleChoiceOption> choices;
    
    Problem(const QString& n = "", const QString& desc = "", const QString& diff = "Easy") 
        : name(n), description(desc), difficulty(diff) {}
};

struct Unit {
    QString name;
    QString description;
    QVector<Problem> problems;
    
    Unit(const QString& n = "", const QString& desc = "") 
        : name(n), description(desc) {}
    
    void addProblem(const Problem& problem) {
        problems.append(problem);
    }
};

class Model : public QObject
{
    Q_OBJECT

public:
    explicit Model(QObject *parent = nullptr);
    
    // Unit management
    void addUnit(const Unit& unit);
    void removeUnit(int index);
    const QVector<Unit>& getUnits() const;
    Unit* getUnit(int index);
    int getUnitCount() const;
    
    // Problem management for specific units
    void addProblemToUnit(int unitIndex, const Problem& problem);
    void removeProblemFromUnit(int unitIndex, int problemIndex);
    QStringList getProblemsForUnit(int unitIndex) const;
    
    // Initialize with sample data
    void initializeSampleData();
    
    // Problem content methods
    QString getProblemStatement(int unitIndex, int problemIndex) const;
    QString getTheoryContent(int unitIndex, int problemIndex) const;
    QVector<MultipleChoiceOption> getMultipleChoiceOptions(int unitIndex, int problemIndex) const;
    int getCorrectChoiceIndex(int unitIndex, int problemIndex) const;
    
    // Utility methods for string conversion
    QString unitProblemToString(int unitIndex, int problemIndex) const;
    std::string qStringToStdString(const QString& qstr) const;
    QString stdStringToQString(const std::string& str) const;

signals:
    void unitsChanged();
    void unitProblemsChanged(int unitIndex);

private:
    QVector<Unit> units;
};

#endif // MODEL_H

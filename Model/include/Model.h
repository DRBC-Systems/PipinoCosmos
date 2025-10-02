#ifndef MODEL_H
#define MODEL_H

#include <QString>
#include <QStringList>
#include <QVector>
#include <QObject>

struct Problem {
    QString name;
    QString description;
    QString difficulty;
    
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

signals:
    void unitsChanged();
    void unitProblemsChanged(int unitIndex);

private:
    QVector<Unit> units;
};

#endif // MODEL_H

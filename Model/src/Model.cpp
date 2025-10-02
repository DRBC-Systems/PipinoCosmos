#include "Model.h"

Model::Model(QObject *parent)
    : QObject(parent)
{
    initializeSampleData();
}

void Model::addUnit(const Unit& unit)
{
    units.append(unit);
    emit unitsChanged();
}

void Model::removeUnit(int index)
{
    if (index >= 0 && index < units.size()) {
        units.removeAt(index);
        emit unitsChanged();
    }
}

const QVector<Unit>& Model::getUnits() const
{
    return units;
}

Unit* Model::getUnit(int index)
{
    if (index >= 0 && index < units.size()) {
        return &units[index];
    }
    return nullptr;
}

int Model::getUnitCount() const
{
    return units.size();
}

void Model::addProblemToUnit(int unitIndex, const Problem& problem)
{
    if (unitIndex >= 0 && unitIndex < units.size()) {
        units[unitIndex].addProblem(problem);
        emit unitProblemsChanged(unitIndex);
    }
}

void Model::removeProblemFromUnit(int unitIndex, int problemIndex)
{
    if (unitIndex >= 0 && unitIndex < units.size()) {
        if (problemIndex >= 0 && problemIndex < units[unitIndex].problems.size()) {
            units[unitIndex].problems.removeAt(problemIndex);
            emit unitProblemsChanged(unitIndex);
        }
    }
}

QStringList Model::getProblemsForUnit(int unitIndex) const
{
    QStringList problemNames;
    if (unitIndex >= 0 && unitIndex < units.size()) {
        for (const Problem& problem : units[unitIndex].problems) {
            problemNames.append(problem.name);
        }
    }
    return problemNames;
}

void Model::initializeSampleData()
{
    // Unit 1: Basic Mathematics
    Unit mathUnit("Basic Mathematics", "Fundamental mathematical concepts and operations");
    mathUnit.addProblem(Problem("Addition & Subtraction", "Basic arithmetic operations", "Easy"));
    mathUnit.addProblem(Problem("Multiplication & Division", "Basic multiplication and division", "Easy"));
    mathUnit.addProblem(Problem("Fractions", "Working with fractions and decimals", "Medium"));
    mathUnit.addProblem(Problem("Percentages", "Calculating percentages and ratios", "Medium"));
    
    // Unit 2: Algebra
    Unit algebraUnit("Algebra", "Introduction to algebraic concepts");
    algebraUnit.addProblem(Problem("Linear Equations", "Solving linear equations", "Medium"));
    algebraUnit.addProblem(Problem("Quadratic Equations", "Solving quadratic equations", "Hard"));
    algebraUnit.addProblem(Problem("Systems of Equations", "Solving systems of linear equations", "Hard"));
    algebraUnit.addProblem(Problem("Polynomials", "Working with polynomial expressions", "Medium"));
    
    // Unit 3: Geometry
    Unit geometryUnit("Geometry", "Basic geometric shapes and calculations");
    geometryUnit.addProblem(Problem("Area & Perimeter", "Calculating area and perimeter", "Easy"));
    geometryUnit.addProblem(Problem("Triangles", "Properties and calculations of triangles", "Medium"));
    geometryUnit.addProblem(Problem("Circles", "Circle properties and calculations", "Medium"));
    geometryUnit.addProblem(Problem("3D Shapes", "Volume and surface area of 3D shapes", "Hard"));
    
    // Unit 4: Statistics
    Unit statsUnit("Statistics", "Basic statistical concepts and data analysis");
    statsUnit.addProblem(Problem("Mean, Median, Mode", "Measures of central tendency", "Easy"));
    statsUnit.addProblem(Problem("Standard Deviation", "Measuring data spread", "Medium"));
    statsUnit.addProblem(Problem("Probability", "Basic probability calculations", "Medium"));
    statsUnit.addProblem(Problem("Data Visualization", "Creating and interpreting graphs", "Easy"));
    
    units.append(mathUnit);
    units.append(algebraUnit);
    units.append(geometryUnit);
    units.append(statsUnit);
}

#include "Model.h"
#include <QDebug>

Model::Model(QObject *parent)
    : QObject(parent), currentUnitIndex(-1), currentProblemIndex(-1), userDifficultySetting("Medium")
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

QString Model::getProblemStatement(int unitIndex, int problemIndex) const
{
    if (unitIndex >= 0 && unitIndex < units.size()) {
        const Unit& unit = units[unitIndex];
        if (problemIndex >= 0 && problemIndex < unit.problems.size()) {
            const Problem& problem = unit.problems[problemIndex];
            return problem.problemStatement;
        }
    }
    return "Problem not found";
}

QString Model::getTheoryContent(int unitIndex, int problemIndex) const
{
    if (unitIndex >= 0 && unitIndex < units.size()) {
        const Unit& unit = units[unitIndex];
        if (problemIndex >= 0 && problemIndex < unit.problems.size()) {
            const Problem& problem = unit.problems[problemIndex];
            if (!problem.theoryContent.isEmpty()) {
                return problem.theoryContent;
            }
            // Generic fallback for theory (AI doesn't generate theory yet)
            return QString("Theory for %1\n\n%2\n\nThis is a %3 level topic in %4.")
                   .arg(problem.name, problem.description, problem.difficulty, unit.name);
        }
    }
    return "Theory content not found";
}

QVector<MultipleChoiceOption> Model::getMultipleChoiceOptions(int unitIndex, int problemIndex) const
{
    if (unitIndex >= 0 && unitIndex < units.size()) {
        const Unit& unit = units[unitIndex];
        if (problemIndex >= 0 && problemIndex < unit.problems.size()) {
            const Problem& problem = unit.problems[problemIndex];
            return problem.choices;
        }
    }
    
    // Return empty if not found - AI should always populate this
    return QVector<MultipleChoiceOption>();
}

int Model::getCorrectChoiceIndex(int unitIndex, int problemIndex) const
{
    QVector<MultipleChoiceOption> choices = getMultipleChoiceOptions(unitIndex, problemIndex);
    for (int i = 0; i < choices.size(); ++i) {
        if (choices[i].isCorrect) {
            return i;
        }
    }
    return 1; // Default to option B if no correct answer found
}

QString Model::unitProblemToString(int unitIndex, int problemIndex) const
{
    if (unitIndex >= 0 && unitIndex < units.size()) {
        const Unit& unit = units[unitIndex];
        if (problemIndex >= 0 && problemIndex < unit.problems.size()) {
            const Problem& problem = unit.problems[problemIndex];
            return QString("%1: %2").arg(unit.name, problem.name);
        }
    }
    return "Invalid unit or problem";
}

std::string Model::qStringToStdString(const QString& qstr) const
{
    return qstr.toStdString();
}

QString Model::stdStringToQString(const std::string& str) const
{
    return QString::fromStdString(str);
}

// Current selection methods implementation
std::string Model::getCurrentProblem() const
{
    // Check if we have a valid current selection
    if (currentUnitIndex >= 0 && currentUnitIndex < units.size() &&
        currentProblemIndex >= 0 && currentProblemIndex < units[currentUnitIndex].problems.size()) {
        
        // Get the current problem name and convert to std::string
        const Problem& problem = units[currentUnitIndex].problems[currentProblemIndex];
        return qStringToStdString(problem.name);
    }
    
    // Return empty string if no valid selection
    return "No problem selected";
}

std::string Model::getCurrentDifficulty() const
{
    // Return the USER'S difficulty setting (from settings), not the problem's hardcoded difficulty
    return qStringToStdString(userDifficultySetting);
}

bool Model::isCurrentProblemMultipleChoice() const
{
    // Check if we have a valid current selection
    if (currentUnitIndex >= 0 && currentUnitIndex < units.size() &&
        currentProblemIndex >= 0 && currentProblemIndex < units[currentUnitIndex].problems.size()) {
        
        // Based on your app logic: problems 0, 1, 2 are multiple choice, problems 3+ are scan problems
        return currentProblemIndex < 3;
    }
    
    // Return false if no valid selection (safer default)
    return false;
}

void Model::setCurrentSelection(int unitIndex, int problemIndex)
{
    // Validate the indices before setting them
    if (unitIndex >= 0 && unitIndex < units.size() &&
        problemIndex >= 0 && problemIndex < units[unitIndex].problems.size()) {
        
        currentUnitIndex = unitIndex;
        currentProblemIndex = problemIndex;
    } else {
        // Invalid selection - reset to no selection
        currentUnitIndex = -1;
        currentProblemIndex = -1;
    }
}

// User settings methods implementation
void Model::setUserDifficulty(const QString& difficulty)
{
    // Validate the difficulty setting
    if (difficulty == "Easy" || difficulty == "Medium" || difficulty == "Hard") {
        userDifficultySetting = difficulty;
    } else {
        // Invalid difficulty - default to Medium
        userDifficultySetting = "Medium";
    }
}

QString Model::getUserDifficulty() const
{
    return userDifficultySetting;
}

void Model::updateProblemContent(int unitIndex, int problemIndex, 
                                 const QString& problemStatement, 
                                 const QVector<MultipleChoiceOption>& choices)
{
    // Validate indices
    if (unitIndex >= 0 && unitIndex < units.size() &&
        problemIndex >= 0 && problemIndex < units[unitIndex].problems.size()) {
        
        // Update the problem's statement and choices
        units[unitIndex].problems[problemIndex].problemStatement = problemStatement;
        units[unitIndex].problems[problemIndex].choices = choices;
        
        qDebug() << "Updated problem content for Unit" << unitIndex << "Problem" << problemIndex;
    }
}